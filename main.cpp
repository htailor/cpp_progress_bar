#include "progress_bar.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main(){
	
	int n;
	
	/// Example 1 ///

	n = 100;
	ProgressBar *bar1 = new ProgressBar(n, "Example 1");
	
	for(int i=0;i<=n;++i){
		bar1->Progressed(i);
        std::this_thread::sleep_for (std::chrono::milliseconds(10));
    }

	/// Example 2 ///

	n = 1000;
	ProgressBar *bar2 = new ProgressBar(n, "Example 2");
	bar2->SetFrequencyUpdate(10);
	bar2->SetStyle("|","-");
	//bar2->SetStyle("\u2588", "-"); for linux
	
	std::cout << std::endl;
	for(int i=0;i<=n;++i){
		bar2->Progressed(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
	
	n = 5;
	ProgressBar bar3(n);
    bar3.Progressed(0);
    std::this_thread::sleep_for (std::chrono::milliseconds(200));
    bar3.Progressed(1);
    std::this_thread::sleep_for (std::chrono::milliseconds(200));
    bar3.Progressed(2);
    std::this_thread::sleep_for (std::chrono::milliseconds(200));
    bar3.Progressed(3);
    std::this_thread::sleep_for (std::chrono::milliseconds(200));
    bar3.Progressed(4);
    std::this_thread::sleep_for (std::chrono::milliseconds(200));
    bar3.Progressed(5);
    // following tests exception error
    std::this_thread::sleep_for (std::chrono::milliseconds(200));
    bar3.Progressed(6);
    std::this_thread::sleep_for (std::chrono::milliseconds(200));
    bar3.Progressed(7);

	return 0;
}
