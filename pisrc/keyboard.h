#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <cstdint>
#include <unordered_map>
#include <vector>

// Manages the keyboard. It's a bunch of GPIO pins.

namespace microsynth_hw {
    // Intended to be used in Keyboard static initializer
    class Keymap {
    public:
        enum class Key: std::uint8_t {
            C1,
            CSharp,
            D,
            DSharp,
            E,
            F,
            FSharp,
            G,
            GSharp,
            A,
            ASharp,
            B,
            C2,

            OctUp,
            OctDown
        };
        // see constructor for GPIO pins
        std::unordered_map<Key, int> key2gpio;

        Keymap();
    };

    class Keyboard {
    public:
        class wiringpi_isr_userdata {
        public:
            Keyboard* kbptr;
            Keymap::Key key;
        };

        using Key = Keymap::Key;
        static Keymap keymap;

    private:
        std::vector<wiringpi_isr_userdata*> all_userdata {};

    public:
        Keyboard();
        ~Keyboard();
    };
}

#endif //KEYBOARD_H
