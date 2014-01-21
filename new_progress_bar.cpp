#ifdef _WINDOWS
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

#include <iostream>
#include <iomanip>
#include <cstring>

#include <thread>
#include <chrono>

#define TOTAL_PERCENTAGE 100.0
#define CHARACTER_WIDTH_PERCENTAGE 4

class ProgressBar{

    public:        
        ProgressBar();
        ProgressBar(int n_, const char *description_);
		
		void SetProgressBarFrequencyUpdate(int frequency_update_);
		void SetProgrsssBarStyle(const char* unit_bar_, const char* unit_space_);		
		int SetProgressBarLengthFromConsoleWidth();
		
        void Progressed(unsigned int idx_);
		
    private:
	
		int n;
        const char *description;

		int desc_width;
		
		const char *unit_bar;
        const char *unit_space;
		
		int frequency_update;

};


ProgressBar::ProgressBar(){}

ProgressBar::ProgressBar(int n_, const char* description_=""){
    
    n = n_;
    frequency_update = n_;
    description = description_;
	
	unit_bar = "=";
	unit_space = " ";
	desc_width = std::strlen(description);	// character width of description field
	
}

void ProgressBar::SetProgressBarFrequencyUpdate(int frequency_update_){
	
	if(frequency_update_ > n){
		frequency_update = n;	 // prevents crash if freq_updates_ > n_
	}
	else{
		frequency_update = frequency_update_;
	}
}

void ProgressBar::SetProgrsssBarStyle(const char* unit_bar_, const char* unit_space_){
	
	unit_bar = unit_bar_;
	unit_space = unit_space_;
}

int ProgressBar::SetProgressBarLengthFromConsoleWidth(){

	// get console width and according adjust the length of the progress bar

	int width;

	#ifdef _WINDOWS
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	#else
		struct winsize win;
		ioctl(0, TIOCGWINSZ, &win);
        width = win.ws_col;
	#endif

    width = static_cast<int>((width - desc_width - CHARACTER_WIDTH_PERCENTAGE) / 2.);

	return width;
}

void ProgressBar::Progressed(unsigned int idx_)
{
    try{
        if(idx_ > n) throw "(PROGRESS_BAR) ERROR: _idx out bounds when _idx > n. \r";

        // determines whether to update the progress bar from frequency_update
	    if ((idx_ != n) && (idx_ % (n/frequency_update) != 0)) return;

        // calculate the size of the progress bar
	    int bar_size = SetProgressBarLengthFromConsoleWidth();
    
        // calculate percentage of progress
        double progress_percent = idx_* TOTAL_PERCENTAGE/n;

        // calculate the percentage value of a unit bar 
        double percent_per_unit_bar = TOTAL_PERCENTAGE/bar_size;

        // display progress bar
	    std::cout << " " << description << " [";

        for(int bar_length=0;bar_length<=bar_size-1;++bar_length){
            if(bar_length*percent_per_unit_bar<progress_percent){
                std::cout << unit_bar;
            }
            else{
                std::cout << unit_space;
            }
        }

        std::cout << "]" << std::setw(CHARACTER_WIDTH_PERCENTAGE) << static_cast<int>(progress_percent) << "%\r" << std::flush;
    }
    catch(const char* e){
        std::cerr << e << std::flush;
    }

}

int main(){

	//int n = 100;
    int n = 5;
	
	ProgressBar *bar = new ProgressBar(n, "Nucleation");
	//bar->SetProgrsssBarStyle("\u2588","-");
	
    bar->Progressed(0);
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    bar->Progressed(1);
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    bar->Progressed(2);
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    bar->Progressed(3);
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    bar->Progressed(4);
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    bar->Progressed(5);

    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    bar->Progressed(6);

    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    bar->Progressed(7);
//    for(int i=0;i<=n;++i){
//		bar->Progressed(i);
   //     std::this_thread::sleep_for (std::chrono::milliseconds(100));
  //  }
	return 0;
}
