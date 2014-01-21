#include "progress_bar.hpp"
#include <thread>
#include <chrono>

int main(){
        
    int n = 100;
    for(int i=0;i<=n;++i){
        ProgressBar(i,n,100);
        std::this_thread::sleep_for (std::chrono::milliseconds(100));
    }
    return 0;
}
