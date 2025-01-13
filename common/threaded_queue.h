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
        threaded_queue();
        ~threaded_queue();

        void put(T& t);

        T getWait();

        template <typename T2>
        [[noreturn]] void stream_to(threaded_queue<T2>& target);
    };
}

#endif //THREADED_QUEUE_H
