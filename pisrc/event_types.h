#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H
#include <chrono>

#include "keyboard.h"

namespace microsynth_hw {
    namespace chrono = std::chrono;
    class event {
    public:
        enum class type_ {
            Key,
            Joystick
        };

        type_ type;
        union {
            KeyEvent key;
        } value;
    };
}

#endif //EVENT_TYPES_H
