#include <iostream>
#include <thread>
#include <wiringPi.h>

#include "libmicrosynth.h"
#include "audio.h"
#include "pi_native.h"
#include "signals.h"

constexpr int PIN_TEST = 17 /* gpio17/header11 */;

namespace chrono = std::chrono;
namespace this_thread = std::this_thread;

using microsynth_hw::Hardware;
namespace Tuning = microsynth::A440;

inline std::string onoff(const int &isItOn) {
    switch (isItOn) {
        case LOW: return "off";
        case HIGH: return "on";
        default: return "idk??";
    }
}

using std::this_thread::sleep_for;
namespace chrono = std::chrono;
namespace Tuning = microsynth::A440;

std::shared_ptr<microsynth::queue_sfx_command> mkqueue(const std::shared_ptr<microsynth::queueable>& it)
{
    return std::make_shared<microsynth::queue_sfx_command>(microsynth::queue_sfx_command { it });
}

int main() {
    Hardware h {};
    microsynth::AudioDriver driver {};
    microsynth::SignalGenerators sig_gen {};
    std::cout << "Hello from Pi.\n";

    const std::shared_ptr c4 { sig_gen.sawtooth(Tuning::C4, 0.25) };
    const std::shared_ptr e4 { sig_gen.sawtooth(Tuning::E4, 0.25) };
    const std::shared_ptr g4 { sig_gen.sawtooth(Tuning::G4, 0.25) };

    driver.enqueue(mkqueue(c4));
    driver.enqueue(mkqueue(e4));
    driver.enqueue(mkqueue(g4));

    sleep_for(chrono::seconds(3));
    driver.finalize();
    return 0;
}
