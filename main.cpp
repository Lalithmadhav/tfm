#include "filemanager.h"
#include <ncurses.h>

int main() {
    FileManager fm;
    initscr();
    keypad(stdscr, TRUE);
    while(true) {
        clear();
        auto entries = fm.list();
        fm.display(entries);
        refresh();
        if (!fm.request(entries)) break;
    }
    endwin();
    return 0;
}
