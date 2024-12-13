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
    constexpr int I2C_SWITCH_DEVICE = 0x0703;

    void worker(const int tid) {
        std::cout << "hi from thread " << tid << std::endl;
    }

    void init() {
        // use 'native' pin numbering (i.e. 17 -> GPIO 17 instead of 0 -> GPIO 17)
        wiringPiSetupGpio();
    }

    Hardware::Hardware() {
        wiringPiSetupGpio();
        i2cfd = wiringPiI2CSetup(LCD_ADDR);
    }

    Hardware::~Hardware() = default;

    bool Hardware::switch_i2c_dev(const int new_device) const {
        if (i2cfd < 0) throw std::runtime_error("No handle on the I2C device was initialized, for some reason");
        if (ioctl(i2cfd, I2C_SWITCH_DEVICE, new_device) < 0) return false;
        return true;
    }
}
