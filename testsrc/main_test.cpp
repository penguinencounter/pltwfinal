#include <iostream>
#include "audio.h"
#include "thread_scratchpad.h"
#include "signals.h"
#include <iomanip>
#include <ncurses.h>

int kbhit() {
    int ch = getch();
    if (ch != ERR) {
        ungetch(ch);
        return 1;
    }
    return 0;
}

void ncurses_preamble() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, true);
    scrollok(stdscr, true);
}

using std::this_thread::sleep_for;
namespace chrono = std::chrono;
namespace Tuning = microsynth::A440;

std::shared_ptr<microsynth::queue_sfx_command> mkqueue(const std::shared_ptr<microsynth::queueable>& it)
{
    return std::make_shared<microsynth::queue_sfx_command>(microsynth::queue_sfx_command { it });
}

std::shared_ptr<microsynth::force_stop_sfx_command> mkstop(const std::shared_ptr<microsynth::queueable>& it)
{
    return std::make_shared<microsynth::force_stop_sfx_command>(microsynth::force_stop_sfx_command { it });
}

int main() {
    std::cout << "hello from c++" << __cplusplus << "!\n";
    microsynth::AudioDriver driver {};

    microsynth::SignalGenerators sig_gen {};
    constexpr int sampleRate = 44100;

    sig_gen.setSampleRate(sampleRate);

    constexpr size_t no_pitches = 9;
    constexpr double pitches[no_pitches] = {
        Tuning::G3,
        Tuning::A3,
        Tuning::B3,
        Tuning::C4,
        Tuning::D4,
        Tuning::E4,
        Tuning::FSharp4,
        Tuning::G4,
        Tuning::A4,
    };

    std::shared_ptr<microsynth::queueable> pitch_actions[no_pitches];
    for (size_t i = 0; i < no_pitches; i++)
        pitch_actions[i] = std::shared_ptr(sig_gen.square(pitches[i], 0.33));

    bool direction = true;
    int i = 0;

    for (;;)
    {
        i += direction ? 1 : -1;
        if (i == 0) direction = !direction;
        if (i == no_pitches - 1) direction = !direction;
        const auto clone = std::shared_ptr(pitch_actions[i]->copy());
        driver.enqueue(mkqueue(clone));
        sleep_for(chrono::milliseconds(100));
        driver.enqueue(mkstop(clone));
    }
}
