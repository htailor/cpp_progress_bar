#ifndef _PROGRESS_BAR_
#define _PROGRESS_BAR_

#ifdef _WINDOWS
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

#include <iostream>
#include <string>


class ProgressBar{
  public:
    ProgressBar(uint64_t total,
                const std::string &description = "",
                std::ostream &out = std::cerr);
    ~ProgressBar();

    void SetFrequencyUpdate(uint64_t frequency_update_);
    void SetStyle(char unit_bar, char unit_space);

    void Progressed(uint64_t idx_);
    ProgressBar& operator++();
    ProgressBar& operator+=(uint64_t delta);

  private:
    ProgressBar(const ProgressBar &) = delete;
    ProgressBar& operator=(const ProgressBar &) = delete;

    uint64_t total_;
    uint64_t progress_ = 0;
    uint64_t frequency_update;
    std::ostream *out;

    std::string description_;
    char unit_bar_ = '=';
    char unit_space_ = ' ';

    void ClearBarField();
    int GetConsoleWidth();
    int GetBarLength();
};

#endif
