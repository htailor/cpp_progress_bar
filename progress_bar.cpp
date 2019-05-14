#include "progress_bar.hpp"

#include <iomanip>
#include <cmath>
#include <cassert>

const size_t kMessageSize = 20;
const double kTotalPercentage = 100.0;
const size_t kCharacterWidthPercentage = 4;


ProgressBar::ProgressBar(uint64_t total,
                         const std::string &description,
                         std::ostream &out_,
                         bool silent)
      : silent_(silent), total_(total), description_(description) {
    if (silent_)
        return;

    frequency_update = std::min(static_cast<uint64_t>(1000), total_);
    out = &out_;
    *out << "\n";

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
    return (GetConsoleWidth() - description_.size()
                                - kCharacterWidthPercentage
                                - std::log10(std::max(uint64_t(1), total_) * 2) * 2) / 2.;
}

void ProgressBar::ClearBarField() const {
    if (silent_)
        return;

    for(int i = 0; i < GetConsoleWidth(); ++i) {
        *out << " ";
    }
    *out << "\r" << std::flush;
}

void ProgressBar::ShowProgress() const {
    if (silent_)
        return;

    std::lock_guard<std::mutex> lock(mu_);

    try {
        // calculate the size of the progress bar
        int bar_size = GetBarLength();

        // calculate percentage of progress
        double progress_percent = total_ ? progress_ * kTotalPercentage / total_
                                         : kTotalPercentage;

        // calculate the percentage value of a unit bar
        double percent_per_unit_bar = kTotalPercentage / bar_size;

        // display progress bar
        // *out << " " << (percent_per_unit_bar < 1
        //                 ? description
        //                 : std::string(' ', desc_width)) << " [";
        *out << " " << description_ << " [";

        for(int bar_length = 0; bar_length <= bar_size - 1; ++bar_length) {
            *out << (bar_length * percent_per_unit_bar < progress_percent
                    ? unit_bar_
                    : unit_space_);
        }

        *out << "]" << std::setw(kCharacterWidthPercentage + 1)
                    << std::setprecision(1)
                    << std::fixed
                    << progress_percent << "%, "
                    << progress_ << "/" << total_ << "\r" << std::flush;
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

    if (after_update > total_)
        throw after_update;

    // determines whether to update the progress bar from frequency_update
    if (after_update == total_ || after_update % (total_ / frequency_update) == 0)
        ShowProgress();

    return *this;
}
