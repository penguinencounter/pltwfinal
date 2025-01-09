#ifndef SIGNALMETA_H
#define SIGNALMETA_H
#include <memory>

namespace microsynth
{
    using signal_fmt = float;
    using signal_ptr = signal_fmt*;
    using signal_buf = signal_fmt[];

    static unsigned long _next_ID = 1;

    class queueable
    {
    public:
        [[nodiscard]] std::unique_ptr<queueable> copy() const;
        friend std::ostream& operator<<(std::ostream& os, queueable const& m);

        unsigned long id = _next_ID++;
        std::shared_ptr<signal_buf> buf;
        size_t length;
        /**
         * When to start looping. Looping only occurs if `use_loop` is true.
         * Otherwise, running off the end of the buffer will erase the queueable.
         */
        size_t loop_at;
        /**
         * When reaching the 'loop_at' position, go to 'loop_to'. Set to 0 to loop the entire thing.
         */
        size_t loop_to;
        size_t position;
        bool use_loop = true;
        bool alive = true;
    };

    /*
     * NOT IMPLEMENTED
     */
    queueable mix(const queueable& first, const queueable& second);
    void reid(queueable& target);
}

#endif //SIGNALMETA_H
