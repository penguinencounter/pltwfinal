#ifndef SIGNALMETA_H
#define SIGNALMETA_H
#include <memory>

namespace microsynth
{
    using signal_fmt = float;
    using signal_ptr = signal_fmt*;
    using signal_buf = signal_fmt[];

    static unsigned long _next_ID = 0;

    struct queueable
    {
        unsigned long id = _next_ID++;
        std::shared_ptr<signal_buf> repeat;
        size_t length;
        size_t position;
    };
}

#endif //SIGNALMETA_H
