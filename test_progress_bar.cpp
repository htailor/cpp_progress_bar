#include "progress_bar.hpp"
#include <thread>
#include <chrono>

int main(){
        
    int n = 20;
    for(int i=0;i<=n;++i){
        ProgressBar(i,n,5,"test");
        std::this_thread::sleep_for (std::chrono::milliseconds(500));
    }
    return 0;
}
