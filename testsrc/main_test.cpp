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

std::shared_ptr<microsynth::QueueSFXCommand> mkqueue(const std::shared_ptr<microsynth::queueable>& it)
{
    return std::make_shared<microsynth::QueueSFXCommand>(microsynth::QueueSFXCommand { it });
}

int main() {
    std::cout << "hello from c++" << __cplusplus << "!\n";
    microsynth::AudioDriver driver {};

    microsynth::SignalGenerators sig_gen {};
    constexpr int sampleRate = 44100;

    sig_gen.setSampleRate(sampleRate);

    const std::shared_ptr c4 { sig_gen.sawtooth(Tuning::C4, 0.25) };
    std::cout << "c4 is id " << c4->id << ", ok\n";
    const std::shared_ptr e4 { sig_gen.sawtooth(Tuning::E4, 0.25) };
    std::cout << "e4 is id " << e4->id << ", ok\n";
    const std::shared_ptr g4 { sig_gen.sawtooth(Tuning::G4, 0.25) };
    std::cout << "g4 is id " << g4->id << ", ok\n";

    driver.enqueue(mkqueue(c4));
    driver.enqueue(mkqueue(e4));
    driver.enqueue(mkqueue(g4));

    sleep_for(std::chrono::seconds(3));
    driver.finalize();
}
