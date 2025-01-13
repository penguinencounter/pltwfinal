#ifndef THREADED_QUEUE_H
#define THREADED_QUEUE_H
#include <condition_variable>
#include <mutex>
#include <queue>

namespace microsynth {
    template<typename T>
    class threaded_queue {
    private:
        std::queue<T> q;
        mutable std::mutex mu;
        std::condition_variable cv;

    public:
        threaded_queue(): q(), mu(), cv() {}
        ~threaded_queue() = default;

        void put(T t) {
            std::lock_guard lock(mu);
            q.push(t);
            // Notify waiting threads that a new item is available.
            cv.notify_one();
        }

        T getWait() {
            std::unique_lock lock(mu);
            while (q.empty()) cv.wait(lock);
            T val = q.front();
            q.pop();
            return val;
        }
    };
}

#endif //THREADED_QUEUE_H
