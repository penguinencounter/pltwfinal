#include <iostream>
#include <thread>
#include <wiringPi.h>

#include "libmicrosynth.h"
#include "audio.h"
#include "ms_ads1115.h"
#include "pi_native.h"
#include "signals.h"
#include "wavimport.h"
#include "keyboard.h"

constexpr int PIN_TEST = 17 /* gpio17/header11 */;

namespace chrono = std::chrono;
namespace this_thread = std::this_thread;

using microsynth_hw::Hardware;
using microsynth_hw::ms_ads1115;
namespace Tuning = microsynth::A440;

inline std::string onoff(const int& isItOn)
{
    switch (isItOn)
    {
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
    return std::make_shared<microsynth::queue_sfx_command>(microsynth::queue_sfx_command{it});
}

void configure_adc(ms_ads1115& adc) {
    std::cout << "\033[92;1mConfiguring ADC...\033[0m\n" << std::flush;
    adc.pull_conf();
    std::cout << "\033[93m Pre-configuration: " << adc.current_configuration << "\033[0m\n";
    ms_ads1115::conf adc_conf = adc.current_configuration;
    adc_conf.data_rate = ms_ads1115::conf::DATA_RATE_8;
    adc_conf.pga = ms_ads1115::conf::PGA_4_096V;
    adc_conf.mode = true; // one-shot mode
    adc_conf.mux = 0b100;
    adc_conf.comp_qd = ms_ads1115::conf::COMP_QD_NOCOMP; // no alert
    if (adc.write_conf(adc_conf) < 0) throw microsynth_hw::communication_failure("failed to write initial configuration to ADC");
    adc.pull_conf();
    std::cout << "\033[93m After configuration: " << adc.current_configuration << "\033[0m\n";
    std::cout << "\033[93;1mchecking... \033[0m" << std::flush;
    adc.spinfetch();
    std::cout << "\033[92mOK\033[0m\n" << std::flush;
}

int main_wrap()
{
    Hardware h{};
    microsynth_hw::Keyboard kbd {};
    [[maybe_unused]] ms_ads1115 adc{h, ms_ads1115::ADDR_GND};
    [[maybe_unused]] microsynth::AudioDriver driver{};
    [[maybe_unused]] microsynth::SignalGenerators sig_gen{};
    configure_adc(adc);

    std::cout << "\033[93mcurrent ADC options: " << adc.fetch_conf() << "\033[0m\n";
    std::cout << "Hello from Pi.\n";

    for (;;) {
        std::int16_t value = adc.analog_read(0);
        double fractional_value = static_cast<double>(value) / static_cast<double>(std::numeric_limits<std::int16_t>::max());
        std::cout << "A0: " << fractional_value << std::endl;
        sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}

int main()
{
    try
    {
        return main_wrap();
    }
    catch (std::exception& except)
    {
        std::cout << "\033[91;1mStopping with an exception:\033[0;91m\n" << except.what() << "\033[0m\n" << std::flush;
        return EXIT_FAILURE;
    }
}
