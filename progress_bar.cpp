#include "progress_bar.hpp"

#include <iomanip>
#include <cmath>
#include <cassert>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <sstream>

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
    #include <unistd.h>
    #if !defined(_POSIX_VERSION)
        #include <io.h>
    #endif
#else
    #include <io.h>
#endif

const size_t kMessageSize = 20;
const double kTotalPercentage = 100.0;
const size_t kCharacterWidthPercentage = 7;
const int kDefaultConsoleWidth = 100;
const int kMaxBarWidth = 100;


bool to_terminal(const std::ostream &os) {
#if _WINDOWS
    if (os.rdbuf() == std::cout.rdbuf() && !_isatty(_fileno(stdout)))
        return false;
    if (os.rdbuf() == std::cerr.rdbuf() && !_isatty(_fileno(stderr)))
        return false;
#else
    if (os.rdbuf() == std::cout.rdbuf() && !isatty(fileno(stdout)))
        return false;
    if (os.rdbuf() == std::cerr.rdbuf() && !isatty(fileno(stderr)))
        return false;
#endif
    return true;
}

ProgressBar::ProgressBar(uint64_t total,
                         const std::string &description,
                         std::ostream &out_,
                         bool silent)
      : silent_(silent), total_(total), description_(description) {

    if (silent_)
        return;

    frequency_update = std::max(static_cast<uint64_t>(1), total_ / 1000);
    out = &out_;

    if ((logging_mode_ = !to_terminal(*out)))
        *out << description_ << std::endl;

    description_.resize(kMessageSize, ' ');

    ShowProgress(0);
}

ProgressBar::~ProgressBar() {
    if (progress_ != total_) {
        // this is not supposed to happen, but may be useful for debugging
        ShowProgress(progress_);
        if (!silent_)
            *out << "\n";
    }
}

void ProgressBar::SetFrequencyUpdate(uint64_t frequency_update_) {
    std::lock_guard<std::mutex> lock(mu_);

    if(frequency_update_ > total_){
        frequency_update = total_;    // prevents crash if freq_updates_ > total_
    } else{
        frequency_update = frequency_update_;
    }
}

void ProgressBar::SetStyle(char unit_bar, char unit_space) {
    std::lock_guard<std::mutex> lock(mu_);

    unit_bar_ = unit_bar;
    unit_space_ = unit_space;
}

int ProgressBar::GetConsoleWidth() const {
    int width = kDefaultConsoleWidth;

#ifdef _WINDOWS
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        width = csbi.srWindow.Right - csbi.srWindow.Left;
#else
        struct winsize win;
        if (ioctl(0, TIOCGWINSZ, &win) != -1)
            width = win.ws_col;
#endif

    return width;
}

int ProgressBar::GetBarLength() const {
    // get console width and according adjust the length of the progress bar
    int max_size = GetConsoleWidth()
                    - 9
                    - description_.size()
                    - kCharacterWidthPercentage
                    - std::ceil(std::log10(std::max(uint64_t(2), total_))) * 4;

    return std::min(max_size / 2, kMaxBarWidth);
}

std::string get_progress_summary(double progress_ratio) {
    std::string buffer = std::string(kCharacterWidthPercentage, ' ');

    // in some implementations, snprintf always appends null terminal character
    snprintf(buffer.data(), kCharacterWidthPercentage,
             "%5.1f%%", progress_ratio * kTotalPercentage);

    // erase the last null terminal character
    buffer.pop_back();
    return buffer;
}

void ProgressBar::ShowProgress(uint64_t progress) const {
    if (silent_)
        return;

    std::lock_guard<std::mutex> lock(mu_);

    // calculate percentage of progress
    double progress_ratio = total_ ? static_cast<double>(progress) / total_
                                   : 1.0;
    assert(progress_ratio >= 0.0);
    assert(progress_ratio <= 1.0);

    if (logging_mode_) {
        // get current time
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                    now.time_since_epoch()) % 1000;
        std::stringstream os;
        os << std::put_time(std::localtime(&time), "[%F %T.")
           << std::setfill('0') << std::setw(3) << ms.count() << "]\t"
           << get_progress_summary(progress_ratio)
           << ", " + std::to_string(progress) + "/" + std::to_string(total_) + '\n';
        *out << os.str() << std::flush;
        return;
    }

    try {
        // clear previous progressbar
        *out << std::string(buffer_.size(), ' ') + '\r' << std::flush;
        buffer_.clear();

        // calculate the size of the progress bar
        int bar_size = GetBarLength();
        if (bar_size < 1)
            return;

        // write the state of the progress bar
        buffer_ = " " + description_
                      + " ["
                        + std::string(size_t(bar_size * progress_ratio), unit_bar_)
                        + std::string(bar_size - size_t(bar_size * progress_ratio), unit_space_)
                      + "] " + get_progress_summary(progress_ratio)
                      + ", " + std::to_string(progress) + "/" + std::to_string(total_) + '\r';

        *out << buffer_ << std::flush;

    } catch (uint64_t e) {
        std::cerr << "PROGRESS_BAR_EXCEPTION: _idx ("
                  << e << ") went out of bounds, greater than total_ ("
                  << total_ << ")." << std::endl << std::flush;
    }
}

ProgressBar& ProgressBar::operator++() {
    return (*this) += 1;
}

ProgressBar& ProgressBar::operator+=(uint64_t delta) {
    if (silent_ || !delta)
        return *this;

    uint64_t after_update
        = progress_.fetch_add(delta, std::memory_order_relaxed) + delta;

    assert(after_update <= total_);

    // determines whether to update the progress bar from frequency_update
    if (after_update == total_
            || (after_update - delta) / frequency_update
                        < after_update / frequency_update)
        ShowProgress(after_update);

    if (after_update == total_)
        *out << std::endl;

    return *this;
}
