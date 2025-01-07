#include <iostream>
#include "libmicrosynth.h"
#include "audio.h"
#include "thread_scratchpad.h"
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

int main() {
    std::cout << "hello from c++" << __cplusplus << "!\n";
    // microsynth::AudioDriver driver {};
    // std::this_thread::sleep_for(std::chrono::seconds(3));
    // driver.finalize();
    ncurses_preamble();
    for (;;) {
        if (kbhit()) {
            printw("acquired the %c\n", static_cast<char>(getch()));
            refresh();
        } else {
            // printw("Nothing yet.\n");
            refresh();
            sleep_for(chrono::milliseconds(10));
        }
    }
}
