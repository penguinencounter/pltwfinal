//
// Created by penguinencounter on 12/10/24.
//

#ifndef LIBMICROSYNTH_H
#define LIBMICROSYNTH_H
#include <string>
#include "portaudio.h"

namespace microsynth {
    void initAudio();

    int countDevices();
}

#endif //LIBMICROSYNTH_H
