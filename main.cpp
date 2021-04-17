#include "progress_bar.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
	/// Example 1 ///
	{
		int n = 100;
		ProgressBar bar1(n, "Example 1");

		for (int i = 1; i <= n; ++i) {
			bar1 += 1;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	/// Example 2 ///
	{
		int n = 1000;
		ProgressBar bar2(n, "Example 2");
		bar2.SetFrequencyUpdate(10);
		bar2.SetStyle('|','-');
		//bar2.SetStyle("\u2588", "-"); for linux
		for (int i = 1; i <= n; ++i) {
			bar2 += 1;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	ProgressBar bar3(5);
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	++bar3;
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	++bar3;
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	++bar3;
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	++bar3;
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	++bar3;
	// following tests exception error
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	++bar3;
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	++bar3;

	return 0;
}
