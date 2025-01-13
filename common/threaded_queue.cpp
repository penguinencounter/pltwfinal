#include "threaded_queue.h"

namespace microsynth {
    template<typename T>
    threaded_queue<T>::threaded_queue(): q() {
    }

    template<typename T>
    threaded_queue<T>::~threaded_queue() = default;

    template<typename T>
    void threaded_queue<T>::put(T &t) {
        std::lock_guard lock(mu);
        q.push(t);
        // Notify waiting threads that a new item is available.
        cv.notify_one();
    }

    template<typename T>
    T threaded_queue<T>::getWait() {
        std::unique_lock lock(mu);
        while (q.empty()) cv.wait(lock);
        T val = q.front();
        q.pop();
        return val;
    }

    template<typename T>
    template<typename T2>
    [[noreturn]] void threaded_queue<T>::stream_to(threaded_queue<T2> &target) {
        static_assert(std::is_base_of_v<T2, T>,
                      "Cannot stream to this container: target value type needs to be same or base of this value type");
        for (;;) {
            target.put(this->getWait());
        }
    }
}
