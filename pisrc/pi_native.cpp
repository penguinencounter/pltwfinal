//
// Created by penguinencounter on 12/11/24.
//

#include "pi_native.h"

namespace microsynth_hw {
    void init() {
        // use 'native' pin numbering (i.e. 17 -> GPIO 17 instead of 0 -> GPIO 17)
        wiringPiSetupGpio();
    }
}
