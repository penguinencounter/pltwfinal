//
// Created by penguinencounter on 12/11/24.
//

#include "pi_native.h"

#include <atomic>
#include <iostream>
#include <thread>
#include <sys/ioctl.h>

#include "wiringPi.h"
#include "wiringPiI2C.h"
#include "helpers.h"

namespace this_thread = std::this_thread;
namespace chrono = std::chrono;

namespace microsynth_hw {
    // *inclusivity!*
    constexpr int I2C_SWITCH_DEVICE = 0x0703;

    void worker(const int tid) {
        std::cout << "hi from thread " << tid << std::endl;
    }

    communication_failure::communication_failure(): runtime_error("General communication failure")
    {
    }

    Hardware::Hardware() {
        wiringPiSetupGpio();
    }

    Hardware::~Hardware() = default;
}
