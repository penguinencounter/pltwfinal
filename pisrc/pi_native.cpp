//
// Created by penguinencounter on 12/11/24.
//

#include "pi_native.h"

#include "wiringPi.h"

namespace microsynth_hw {
    void init() {
        wiringPiSetup();
        wiringPiSetupGpio();
    }
}
