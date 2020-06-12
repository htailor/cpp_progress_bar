#ifndef _PROGRESS_BAR_
#define _PROGRESS_BAR_

#ifdef _WINDOWS
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

#include <iostream>
#include <string>
#include <mutex>
#include <atomic>


class ProgressBar {
  public:
    ProgressBar(uint64_t total,
                const std::string &description = "",
                std::ostream &out = std::cerr,
                bool silent = false);

    ~ProgressBar();

    void SetFrequencyUpdate(uint64_t frequency_update_);
    void SetStyle(char unit_bar, char unit_space);

    ProgressBar& operator++();
    ProgressBar& operator+=(uint64_t delta);

  private:
    ProgressBar(const ProgressBar &) = delete;
    ProgressBar& operator=(const ProgressBar &) = delete;

    void ShowProgress(uint64_t progress) const;
    int GetConsoleWidth() const;
    int GetBarLength() const;

    bool silent_;
    bool logging_mode_;
    uint64_t total_;
    std::atomic<uint64_t> progress_ = 0;
    uint64_t frequency_update;
    std::ostream *out;
    mutable std::mutex mu_;
    mutable std::string buffer_;

    std::string description_;
    char unit_bar_ = '=';
    char unit_space_ = ' ';
};

#endif // _PROGRESS_BAR_
