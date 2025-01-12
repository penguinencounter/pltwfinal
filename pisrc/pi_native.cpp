//
// Created by penguinencounter on 12/11/24.
//

#include "pi_native.h"

#include <iostream>
#include <thread>

#include "wiringPi.h"
#include "helpers.h"

namespace this_thread = std::this_thread;
namespace chrono = std::chrono;

namespace microsynth_hw {
    void worker(const int tid) {
        std::cout << "hi from thread " << tid << std::endl;
    }

    communication_failure::communication_failure(): runtime_error("General communication failure")
    {
    }

    communication_failure::communication_failure(const std::string& extra): runtime_error(
        "Communication failure: " + extra
        )
    {
    }

    Hardware::Hardware() {
        wiringPiSetupGpio();
    }

    Hardware::~Hardware() = default;
}
