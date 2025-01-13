#include "keyboard.h"

#include <functional>
#include <iomanip>
#include <ostream>
#include <wiringPi.h>
#include <iostream>
#include <linux/gpio.h>

namespace microsynth_hw {
    using enum Keymap::Key;

    Keymap::Keymap(): key2gpio({
        {C1, 22},
        {CSharp, 10},
        {D, 9},
        {DSharp, 11},
        {E, 5},
        {F, 6},
        {FSharp, 13},
        {G, 19},
        {GSharp, 26},
        {A, 12},
        {ASharp, 16},
        {B, 20},
        {C2, 21},

        {OctUp, 17},
        {OctDown, 27},
        {Ctrl1, 15},
        {Ctrl2, 18},
        {Ctrl3, 23},
    }) {
    }

    Keymap Keyboard::keymap{};

    volatile static bool is_in_setup = false;

    void isr_handler(void* userdata) {
        if (is_in_setup) return;
        auto* data = static_cast<Keyboard::wiringpi_isr_userdata*>(userdata);
        Keyboard* that = data->kbptr;
        bool pressed = digitalRead(data->pin) == LOW;
        if (pressed != that->key_state[data->key]) {
            std::cout << "update:" << static_cast<std::uint8_t>(data->key) << " on pin " << data->pin  << " " << (pressed ? "pressed" : "released") << std::endl;
            that->key_state[data->key] = pressed;
        }
    }

    Keyboard::Keyboard() {
        is_in_setup = true;
        std::cout << "\033[92;1mSetting up keyboard interrupts, this might take a bit...\033[0m" << std::endl;
        std::size_t i = 0;
        const std::size_t max = keymap.key2gpio.size();
        for (auto& [key, gpio] : keymap.key2gpio) {
            i++;
            std::stringstream ss {};
            ss << "\r\033[93m" << i << " of " << max << "...\033[0m";
            std::cout << std::setw(16) << ss.str() << std::flush;
            auto* data = new wiringpi_isr_userdata {
                this, key, gpio
            };
            all_userdata.push_back(data);
            pinMode(gpio, INPUT);
            // connect to GND to trigger
            pullUpDnControl(gpio, PUD_UP);
            key_state[key] = digitalRead(gpio) == LOW;
            wiringPiISR(gpio, GPIOEVENT_REQUEST_BOTH_EDGES, isr_handler, data);
        }
        std::cout << "\033[92mdone\033[0m" << std::endl;
        is_in_setup = false;
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
