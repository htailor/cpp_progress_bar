#include "progress_bar.hpp"

#include <iomanip>
#include <cmath>
#include <cassert>
#include <cstdio>

const size_t kMessageSize = 20;
const double kTotalPercentage = 100.0;
const size_t kCharacterWidthPercentage = 7;


ProgressBar::ProgressBar(uint64_t total,
                         const std::string &description,
                         std::ostream &out_,
                         bool silent)
      : silent_(silent), total_(total), description_(description) {
    if (silent_)
        return;

    frequency_update = std::min(static_cast<uint64_t>(1000), total_);
    out = &out_;

    assert(description_.size() <= kMessageSize);
    description_.resize(kMessageSize, ' ');

    ShowProgress();
}

ProgressBar::~ProgressBar() {
    if (!silent_)
        *out << "\n";
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
    int width;

    #ifdef _WINDOWS
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        width = csbi.srWindow.Right - csbi.srWindow.Left;
    #else
        struct winsize win;
        ioctl(0, TIOCGWINSZ, &win);
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
                    - std::ceil(std::log10(std::max(uint64_t(1), total_))) * 2;

    return max_size / 2;
}

void ProgressBar::ClearBarField() const {
    if (!silent_)
        *out << (std::string(GetConsoleWidth(), ' ') + '\r') << std::flush;
}

void ProgressBar::ShowProgress() const {
    if (silent_)
        return;

    std::lock_guard<std::mutex> lock(mu_);

    try {
        // clear previous progressbar
        *out << std::string(buffer_.size(), ' ') + '\r' << std::flush;
        buffer_.clear();

        // calculate the size of the progress bar
        int bar_size = GetBarLength();
        if (bar_size < 1)
            return;

        // calculate percentage of progress
        double progress_ratio = total_ ? static_cast<double>(progress_) / total_
                                       : 1.0;
        assert(progress_ratio >= 0.0);
        assert(progress_ratio <= 1.0);

        // write the state of the progress bar
        buffer_ = " " + description_
                      + " ["
                        + std::string(size_t(bar_size * progress_ratio), unit_bar_)
                        + std::string(bar_size - size_t(bar_size * progress_ratio), unit_space_)
                      + "] " + std::string(kCharacterWidthPercentage, ' ');

        snprintf(&buffer_[buffer_.size() - kCharacterWidthPercentage],
                 kCharacterWidthPercentage,
                 "%5.1f%%", progress_ratio * kTotalPercentage);

        buffer_ += ", " + std::to_string(progress_) + "/" + std::to_string(total_) + '\r';

        *out << buffer_ << std::flush;

    } catch (uint64_t e) {
        ClearBarField();
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

    uint64_t after_update = (progress_ += delta);

    assert(after_update <= total_);

    // determines whether to update the progress bar from frequency_update
    if (after_update == total_
            || (after_update - delta) / frequency_update
                        < after_update / frequency_update)
        ShowProgress();

    return *this;
}
