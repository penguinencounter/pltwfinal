#include <iostream>
#include <thread>
#include <wiringPi.h>

#include "libmicrosynth.h"
#include "pi_native.h"

constexpr int PIN_TEST = 0 /* gpio17/header11 */;

namespace chrono = std::chrono;
namespace this_thread = std::this_thread;

inline std::string onoff(const int &isItOn) {
    switch (isItOn) {
        case LOW: return "off";
        case HIGH: return "on";
        default: return "idk??";
    }
}

int main() {
    microsynth_hw::init();
    microsynth::initAudio();
    std::cout << "Hello, world!\n";
    std::cout << "There are " << microsynth::countDevices() << " devices." << std::endl;

    pinMode(PIN_TEST, INPUT);
    pullUpDnControl(PIN_TEST, PUD_DOWN);
    for (;;) {
        std::this_thread::sleep_for(chrono::milliseconds(500));
        std::cout << "The current state of GPIO 17 is: " << onoff(digitalRead(PIN_TEST)) << std::endl;
    }

    return 0;
}
