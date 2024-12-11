#include <iostream>
#include "libmicrosynth.h"

int main() {
    microsynth::initAudio();
    std::cout << "there are " << microsynth::countDevices() << " devices" << std::endl;
}
