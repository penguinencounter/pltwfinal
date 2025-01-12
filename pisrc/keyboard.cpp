#include "keyboard.h"

#include <functional>
#include <ostream>
#include <wiringPi.h>
#include <c++/11/iostream>
#include <linux/gpio.h>

namespace microsynth_hw {
    using enum Keymap::Key;

    Keymap::Keymap(): key2gpio({
        {C1, 17},
        {CSharp, 27},
        {D, 22},
        {DSharp, 10},
        {E, 9},
        {F, 11},
        {FSharp, 5},
        {G, 6},
        {GSharp, 13},
        {A, 19},
        {ASharp, 26},
        {B, 20},
        {C2, 21},

        {OctUp, 23},
        {OctDown, 24}
    }) {
    }

    Keymap Keyboard::keymap{};

    void isr_handler(void* userdata) {
        auto* data = static_cast<Keyboard::wiringpi_isr_userdata*>(userdata);
        std::cout << static_cast<std::uint8_t>(data->key) << std::endl;
    }

    Keyboard::Keyboard() {
        for (auto& [key, gpio] : keymap.key2gpio) {
            auto* data = new wiringpi_isr_userdata {
                this, key
            };
            all_userdata.push_back(data);
            wiringPiISR(gpio, GPIOEVENT_REQUEST_BOTH_EDGES, isr_handler, data);
        }
    }

    Keyboard::~Keyboard() {
        for (auto& [key, gpio] : keymap.key2gpio) {
            wiringPiISRStop(gpio);
        }
        for (const wiringpi_isr_userdata* data_ptr : all_userdata) {
            delete data_ptr;
        }
    }
}
