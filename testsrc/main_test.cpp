#include <iostream>
#include "audio.h"
#include "thread_scratchpad.h"
#include "signals.h"
#include <iomanip>
#include <ncurses.h>

#include "wavimport.h"

struct WAVE_HEADER {
    char Chunk[4];
    int ChunkSize;
    char format[4];
    char Sub_chunk1ID[4];
    int Sub_chunk1Size;
    short int AudioFormat;
    short int NumChannels;
    int SampleRate;
    int ByteRate;
    short int BlockAlign;
    short int BitsPerSample;
    char Sub_chunk2ID[4];
    int Sub_chunk2Size;
};

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

std::shared_ptr<microsynth::req_stop_sfx_command> mkstop(const std::shared_ptr<microsynth::queueable>& it)
{
    return std::make_shared<microsynth::req_stop_sfx_command>(microsynth::req_stop_sfx_command { it });
}

[[noreturn]] void scales(microsynth::AudioDriver& driver, const microsynth::SignalGenerators& sig_gen)
{
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

void test_fadeout(microsynth::AudioDriver& driver, const microsynth::SignalGenerators& sig_gen)
{
    std::shared_ptr c = sig_gen.add_tail(sig_gen.square(Tuning::C4, 0.8));
    std::cout << *c << "\n";
    driver.enqueue(mkqueue(c));
    sleep_for(chrono::milliseconds(500));
    driver.enqueue(mkstop(c));
    sleep_for(chrono::milliseconds(1000));
}

void test_read(microsynth::AudioDriver& driver, const microsynth::SignalGenerators&)
{
    std::shared_ptr c = extras::import_wav("astley.priv.wav");
    driver.enqueue(mkqueue(c));
    sleep_for(chrono::seconds(11));
}

int main() {
    std::cout << "hello from c++" << __cplusplus << "!\n";
    microsynth::AudioDriver driver {};

    microsynth::SignalGenerators sig_gen {};
    constexpr int sampleRate = 44100;
    sig_gen.setSampleRate(sampleRate);

    // test_fadeout(driver, sig_gen);
    test_read(driver, sig_gen);
}
