//
// Created by penguinencounter on 12/12/24.
//

#include "thread_scratchpad.h"

#include <iostream>


namespace this_thread = std::this_thread;
namespace chrono = std::chrono;

void TestThreads::worker(const TestThreads* that) {
    while (!that->sigstop.load()) {
        std::cout << "hi from the thread" << std::endl;
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    std::cout << "thread: bye!" << std::endl;
}

void TestThreads::init() {
    std::cout << "TestThreads init" << std::endl;
    sigstop.store(false);
    worker_thread = std::thread(worker, this);
    std::cout << "TestThreads init done" << std::endl;
}

TestThreads::~TestThreads() {
    std::cout << "TestThreads destr" << std::endl;
    sigstop.store(true);
    if (worker_thread.joinable()) worker_thread.join();
    std::cout << "TestThreads destr done" << std::endl;
}
