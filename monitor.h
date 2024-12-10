#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

class Monitor {
public:
    Monitor() : ready(false), stop(false) {}

    void provide() {
        std::lock_guard<std::mutex> lock(mtx); 
        if (ready) {
            return; 
        }
        ready = true; 
        std::cout << "Provider: Event triggered!\n";
        cv.notify_one(); 
    }

    void consume() {
        std::unique_lock<std::mutex> lock(mtx); 
        while (!ready) { 
            cv.wait(lock); 
        }
        std::cout << "Consumer: Event received and processed!\n";
        ready = false; 
    }

    void stopMonitor() {
        stop = true;
        cv.notify_all(); 
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    bool ready;
    bool stop;
};