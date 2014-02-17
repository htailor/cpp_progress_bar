C++ Progress Bar
================

C++ class that implements a progress bar in console applications on both Linux and Windows platforms. The progress bar works by referencing an index that keeps updating when processing through a piece of code.

Key features
-------------

1. Control the number of updates the progress bar makes.
2. The ability to adjust the style of the progress bar.
3. The length of the progress bar displayed on screen adapts to the the width of the console.

Implementation
===========

To use the progress bar, include the following header file:

```C++
#include "progress_bar.hpp"
```

Creating a progress bar
------------------------

To create a progress bar, the total number of jobs need to be known.

```C++   
int n = 10;
ProgressBar progress_bar(n);
```
    
 Optionally, small descriptions can also be added to the progress bar

```C++ 
int n = 10;
ProgressBar progress_bar(n,"Example 1");
```
 
 
 
Updating a progress bar
-------------------------
 
Updates to the progress bar are made using the `Progressed(int)` method by passing the index of the job
 

**Example 1:**

```C++
int n = 10;
ProgressBar progress_bar(n,"Example 1");
    
int job_index = 0;
    
do_some_work();
    
job_index++;
progress_bar.Progressed(job_index);
 
do_some_more_work();
    
job_index++;
progress_bar.Progressed(job_index);
```
 
**Example 2:**

```C++
int n = 100;
ProgressBar *bar = new ProgressBar(n, "Example 1");
	
for(int i=0;i<=n;++i){
	bar1->Progressed(i);
	std::this_thread::sleep_for (std::chrono::milliseconds(10));
}
```

Setting the frequency of updates
----------------------------------

If progressing through a large job list, iterating through every update will slow down the program. Using the method `SetFrequencyUpdate(int freq)`  overcomes this problem by setting how many times the progress bar should be updated.

**Example 3:**
```C++
int n = 100000;
ProgressBar *bar2 = new ProgressBar(n, "Example 3");
bar2->SetFrequencyUpdate(10);
	
for(int i=0;i<=n;++i){
	bar2->Progressed(i);
}
```

Changing the bar style
------------------------

The progress bar can be customised using the `SetStyle(const char*, const char*)` method. This changes the ASCII/Unicode characters used for printing the progress bar.

**Example 4**
```C++
n = 1000;
ProgressBar *bar3 = new ProgressBar(n, "Example 4");
bar3->SetFrequencyUpdate(10);
bar3->SetStyle("\u2588", "-");
	
for(int i=0;i<=n;++i){
	bar3->Progressed(i);
}
```


Main Example
=========


```C++
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
```






