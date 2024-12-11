#include <iostream>
#include <wiringPi.h>

#include "libmicrosynth.h"

int main() {
    wiringPiSetup();
    microsynth::initAudio();
    std::cout << "Hello, world!\n";
    std::cout << "there are " << microsynth::countDevices() << " devices." << std::endl;
    return 0;
}
