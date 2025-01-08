#include <iostream>
#include "libmicrosynth.h"
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

int main() {
    std::cout << "hello from c++" << __cplusplus << "!\n";
    microsynth::AudioDriver driver {};

    microsynth::SignalGenerators sig_gen {};
    constexpr int sampleRate = 44100;

    sig_gen.setSampleRate(sampleRate);

    const std::shared_ptr that { sig_gen.sine(Tuning::C4, 0.5) };
    driver.enqueue(std::make_shared<microsynth::QueueSFXCommand>(microsynth::QueueSFXCommand {that}));

    sleep_for(std::chrono::seconds(3));
    driver.finalize();
}
