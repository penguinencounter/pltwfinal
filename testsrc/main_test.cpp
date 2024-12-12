#include <iostream>
#include "libmicrosynth.h"
#include "thread_scratchpad.h"

int main() {
    microsynth::initAudio();
    std::cout << "there are " << microsynth::countDevices() << " devices" << std::endl;
    TestThreads testbed;
    testbed.init();
    std::this_thread::sleep_for(std::chrono::seconds(3));
}
