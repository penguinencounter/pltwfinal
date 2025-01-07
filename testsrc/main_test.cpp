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
    sleep_for(std::chrono::seconds(3));
    driver.finalize();

    microsynth::SignalGenerators sig_gen {};
    constexpr int sampleRate = 44100;

    sig_gen.setSampleRate(sampleRate);

    std::unique_ptr that { sig_gen.sawtooth(Tuning::C4) };

    // std::cout << "[\n";
    for (size_t i = 0; i < sampleRate; i++) {
        std::cout << "" << i << ", " << that[i] << "\n";
    }
    // std::cout << "]\n";

    // ncurses_preamble();
    // for (;;) {
    //     if (kbhit()) {
    //         printw("acquired the %c\n", static_cast<char>(getch()));
    //         refresh();
    //     } else {
    //         // printw("Nothing yet.\n");
    //         refresh();
    //         sleep_for(chrono::milliseconds(10));
    //     }
    // }
}
