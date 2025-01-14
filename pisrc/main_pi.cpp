#include <functional>
#include <iostream>
#include <thread>
#include <wiringPi.h>

#include "libmicrosynth.h"
#include "audio.h"
#include "event_types.h"
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

std::shared_ptr<microsynth::queue_sfx_command> mkqueue(const std::shared_ptr<microsynth::generic_clip> &it) {
    return std::make_shared<microsynth::queue_sfx_command>(microsynth::queue_sfx_command{it});
}

std::shared_ptr<microsynth::req_stop_sfx_command> mkreqstop(const std::shared_ptr<microsynth::generic_clip> &it) {
    return std::make_shared<microsynth::req_stop_sfx_command>(microsynth::req_stop_sfx_command{it});
}

std::shared_ptr<microsynth::req_stop_sfx_command> mkreqstop(unsigned long it) {
    return std::make_shared<microsynth::req_stop_sfx_command>(microsynth::req_stop_sfx_command{it});
}

void configure_adc(ms_ads1115 &adc) {
    std::cout << "\033[92;1mConfiguring ADC...\033[0m\n" << std::flush;
    adc.pull_conf();
    std::cout << "\033[93m Pre-configuration: " << adc.current_configuration << "\033[0m\n";
    ms_ads1115::conf adc_conf = adc.current_configuration;
    adc_conf.data_rate = ms_ads1115::conf::DATA_RATE_8;
    adc_conf.pga = ms_ads1115::conf::PGA_4_096V;
    adc_conf.mode = true; // one-shot mode
    adc_conf.mux = 0b100;
    adc_conf.comp_qd = ms_ads1115::conf::COMP_QD_NOCOMP; // no alert
    if (adc.write_conf(adc_conf) < 0)
        throw microsynth_hw::communication_failure(
            "failed to write initial configuration to ADC");
    adc.pull_conf();
    std::cout << "\033[93m After configuration: " << adc.current_configuration << "\033[0m\n";
    std::cout << "\033[93;1mchecking... \033[0m" << std::flush;
    adc.spinfetch();
    std::cout << "\033[92mOK\033[0m\n" << std::flush;
}

template<typename T1, typename T2 = std::shared_ptr<T1> >
void wrap_and_transfer(microsynth_hw::event::type_ kind, microsynth::threaded_queue<T2> &source,
                       microsynth::threaded_queue<std::shared_ptr<microsynth_hw::event> > &dest) {
    // Run in a thread :P
    for (;;) {
        dest.put(std::make_shared<microsynth_hw::event>(microsynth_hw::event{
            .type = kind,
            .value = *source.getWait()
        }));
    }
}

using Key = microsynth_hw::Keymap::Key;
static constexpr int MAX_OCTAVE_OFFSET = 3;
static constexpr int MIN_OCTAVE_OFFSET = -4;

int main_wrap() {
    Hardware h{};
    microsynth_hw::Keyboard kbd{};
    [[maybe_unused]] ms_ads1115 adc{h, ms_ads1115::ADDR_GND};
    [[maybe_unused]] microsynth::AudioDriver driver{};
    [[maybe_unused]] microsynth::SignalGenerators sig_gen{};

    std::shared_ptr rickroll { extras::import_wav("astley.priv.wav") };

    sig_gen.setSampleRate(44100);

    microsynth::threaded_queue<std::shared_ptr<microsynth_hw::event> > all_queue{};
    std::thread key_queue_proc{
        [&] {
            wrap_and_transfer<microsynth_hw::KeyEvent>(microsynth_hw::event::type_::Key, kbd.event_queue, all_queue);
        }
    };
    std::unordered_map<microsynth_hw::Keymap::Key, unsigned long> key_id{};
    for (auto &[key, gpio_]: microsynth_hw::Keyboard::keymap.key2gpio)
        key_id[key] = 0;

    std::cout << "Event loop started\n";

    int octave_selected = 0;
    std::string key_names[] = {
        "C",
        "C#",
        "D",
        "D#",
        "E",
        "F",
        "F#",
        "G",
        "G#",
        "A",
        "A#",
        "B",
        "C"
    };

    for (;;) {
        switch (std::shared_ptr event_ptr{all_queue.getWait()}; event_ptr->type) {
            case microsynth_hw::event::type_::Key: {
                microsynth_hw::KeyEvent &ke{event_ptr->value.key};
                if (ke.key >= Key::C1 && ke.key <= Key::C2) {
                    // Note keys
                    if (ke.kind == microsynth_hw::KeyEvent::Kind::KEY_UP) {
                        std::cout << "Stopping key " << static_cast<std::uint8_t>(ke.key) << "\n";
                        if (key_id.contains(ke.key))
                            driver.enqueue(mkreqstop(key_id[ke.key]));
                    } else if (ke.kind == microsynth_hw::KeyEvent::Kind::KEY_DOWN) {
                        std::int8_t semitones = static_cast<std::int8_t>(ke.key) - static_cast<std::int8_t>(
                                                    Key::A);
                        double pitch =
                                microsynth::factor(
                                    microsynth::factor(Tuning::A4, Tuning::SEMITONE, semitones),
                                    Tuning::OCTAVE,
                                    octave_selected
                                );
                        std::cout << "play key " << key_names[static_cast<std::uint8_t>(ke.key)] << octave_selected + 4 << "\n";
                        std::shared_ptr tone{
                            sig_gen.multisine({
                                                  pitch, pitch / 2.0 - 1, pitch / 3.0 - 2,
                                                  pitch * 2.0 + 1, pitch * 3.0 + 2
                                              }, 0.4)
                        };
                        key_id[ke.key] = tone->id;
                        driver.enqueue(mkqueue(tone));
                    }
                }
                if (ke.kind == microsynth_hw::KeyEvent::Kind::KEY_DOWN) {
                    switch (ke.key) {
                        case Key::OctUp: {
                            octave_selected++;
                            if (octave_selected > MAX_OCTAVE_OFFSET) octave_selected = MAX_OCTAVE_OFFSET;
                            std::cout << "octave " << octave_selected + 4 << "\n";
                            break;
                        }
                        case Key::OctDown: {
                            octave_selected--;
                            if (octave_selected < MIN_OCTAVE_OFFSET) octave_selected = MIN_OCTAVE_OFFSET;
                            std::cout << "octave " << octave_selected + 4 << "\n";
                            break;
                        }
                        default: break;
                    }
                    switch (ke.key) {
                        case Key::OctUp:
                        case Key::OctDown: {
                            if (kbd.key_state[Key::OctUp] && kbd.key_state[Key::OctDown]) driver.enqueue(mkqueue(rickroll->copy()));
                            break;
                        }
                        default: break;
                    }
                }
                break;
            }
            case microsynth_hw::event::type_::Joystick: {
                break;
            }
        }
    }
    return 0;
}

int main() {
    try {
        return main_wrap();
    } catch (std::exception &except) {
        std::cout << "\033[91;1mStopping with an exception:\033[0;91m\n" << except.what() << "\033[0m\n" << std::flush;
        return EXIT_FAILURE;
    }
}
