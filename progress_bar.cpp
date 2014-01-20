#ifdef _WINDOWS
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

#include "progress_bar.hpp"
#include <iostream>
#include <iomanip>


void ProgressBar(unsigned int idx_, const unsigned int n_, unsigned int freq_updates_, const char *desc_)
{
    if(freq_updates_ > n_){ freq_updates_ = n_; } // prevents crash if freq_updates_ > n_

	if ( (idx_ % (n_/freq_updates_) != 0) ) return;

	static int desc_width = 1;    // character width of description field
    static int percent_width = 4; // character width of percentage field

    // get console width and according adjust the length of the progress bar
	
	int bar_size = 100;	// default bar size set to 100 characters
	
	#ifdef _WINDOWS
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		int columns;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		bar_size = static_cast<int>((columns - desc_width - percent_width) / 2.);
	#else
		struct winsize win;
		ioctl(0, TIOCGWINSZ, &win);
		bar_size = static_cast<int>((win.ws_col - desc_width - percent_width) / 2.);
	#endif
	
    // calculate percentage of progress
	static double total_percentage = 100.0;
    double progress_percent = idx_*total_percentage/n_;
     
    // calculate the percentage value of a unit bar 
    double percent_per_unit_bar = total_percentage/bar_size;

    // display progress bar
	std::cout << std::setw(desc_width) << desc_ << " [";

    for(int bar_length=0;bar_length<=bar_size-1;++bar_length){
        if(bar_length*percent_per_unit_bar<progress_percent){
            std::cout << "=";
        }
        else{
            std::cout << " ";
        }
    }

    std::cout << "]" << std::setw(percent_width) << static_cast<int>(progress_percent) << "%\r" << std::flush;
}