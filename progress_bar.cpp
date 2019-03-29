#include "progress_bar.hpp"

#include <iomanip>

const size_t kMessageSize = 25;
const double kTotalPercentage = 100.0;
const size_t kCharacterWidthPercentage = 4;


ProgressBar::ProgressBar(uint64_t total,
                         const std::string &description,
                         std::ostream &out_)
      : total_(total), description_(description) {
    frequency_update = std::min(static_cast<uint64_t>(1000), total_);
    out = &out_;
    *out << "\n";

    assert(description_.size() <= kMessageSize);
    description_.resize(kMessageSize, ' ');
}

ProgressBar::~ProgressBar() {
    *out << "\n";
}

void ProgressBar::SetFrequencyUpdate(uint64_t frequency_update_) {
    if(frequency_update_ > total_){
        frequency_update = total_;    // prevents crash if freq_updates_ > total_
    } else{
        frequency_update = frequency_update_;
    }
}

void ProgressBar::SetStyle(char unit_bar, char unit_space){
    unit_bar_ = unit_bar;
    unit_space_ = unit_space;
}

int ProgressBar::GetConsoleWidth() {
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

int ProgressBar::GetBarLength() {
    // get console width and according adjust the length of the progress bar
    return (GetConsoleWidth() - description_.size()
                                - kCharacterWidthPercentage) / 2.;
}

void ProgressBar::ClearBarField() {
    for(int i = 0; i < GetConsoleWidth(); ++i) {
        *out << " ";
    }
    *out << "\r" << std::flush;
}

void ProgressBar::Progressed(uint64_t idx_) {
    try {
        if (idx_ > total_)
            throw idx_;

        progress_ = idx_;

        // determines whether to update the progress bar from frequency_update
        if ((idx_ != total_) && (idx_ % (total_ / frequency_update) != 0))
            return;

        // calculate the size of the progress bar
        int bar_size = GetBarLength();

        // calculate percentage of progress
        double progress_percent = idx_* kTotalPercentage / total_;

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
                    << progress_percent << "%\r" << std::flush;
    } catch (uint64_t e) {
        ClearBarField();
        std::cerr << "PROGRESS_BAR_EXCEPTION: _idx ("
                  << e << ") went out of bounds, greater than total_ ("
                  << total_ << ")." << std::endl << std::flush;
    }
}

ProgressBar& ProgressBar::operator++() {
    this->Progressed(progress_ + 1);
    return *this;
}
