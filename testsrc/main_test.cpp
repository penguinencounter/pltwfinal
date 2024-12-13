#include <iostream>
#include "libmicrosynth.h"
#include "audio.h"
#include "thread_scratchpad.h"

int main() {
    std::cout << "hello from c++" << __cplusplus << "!\n";
    microsynth::AudioDriver driver {};
    std::this_thread::sleep_for(std::chrono::seconds(3));
    driver.finalize();
}
