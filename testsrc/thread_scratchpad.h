//
// Created by penguinencounter on 12/12/24.
//

#ifndef THREAD_SCRATCHPAD_H
#define THREAD_SCRATCHPAD_H

#include <thread>
#include <atomic>

class TestThreads {
private:
    std::thread worker_thread {};
    std::atomic<bool> sigstop {false};

    static void worker(const TestThreads* that);
public:
    void init();
    ~TestThreads();
};

#endif //THREAD_SCRATCHPAD_H
