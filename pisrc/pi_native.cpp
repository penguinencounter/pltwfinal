//
// Created by penguinencounter on 12/11/24.
//

#include "pi_native.h"

#include <atomic>
#include <iostream>
#include <thread>

#include "wiringPi.h"

namespace this_thread = std::this_thread;
namespace chrono = std::chrono;

namespace microsynth_hw {
    class Hardware {
    private:
        std::thread worker_thread {};
        std::atomic<bool> sigstop{ false };

        static void worker(const Hardware* that) {
            while (!that->sigstop.load()) {
                this_thread::sleep_for(chrono::milliseconds(5));
            }
        }

    public:
        void init() {
            wiringPiSetupGpio();
            sigstop.store(false);
            worker_thread = std::thread(worker, this);
        }

        ~Hardware() {
            sigstop.store(true);
            if (worker_thread.joinable()) worker_thread.join();
        }
    };

    void worker(const int tid) {
        std::cout << "hi from thread " << tid << std::endl;
    }

    void init() {
        // use 'native' pin numbering (i.e. 17 -> GPIO 17 instead of 0 -> GPIO 17)
        wiringPiSetupGpio();
    }

}
