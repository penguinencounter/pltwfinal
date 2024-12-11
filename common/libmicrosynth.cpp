//
// Created by penguinencounter on 12/10/24.
//

#include "libmicrosynth.h"

namespace microsynth {
    void initAudio() {
        Pa_Initialize();
    }


    int countDevices() {
        return Pa_GetDeviceCount();
    }
}
