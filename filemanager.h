#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <algorithm>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <ncurses.h>

namespace fs = std::filesystem;

class FileManager {
private:
    fs::path current_path;
    int selected = 0;
    int scroll_offset = 0;

public: 
    std::vector<fs::path>list() {
        std::vector<fs::path> entries;
        for (const auto& dir_entry : fs::directory_iterator(current_path)) {
            entries.push_back(dir_entry.path());
        }
        return entries;
    }

    FileManager() : current_path(fs::current_path()) {};

    bool request(const std::vector<fs::path>& entries) {
        int key = getch();
        if (key == KEY_UP && selected > 0) selected--;
        else if (key == KEY_DOWN && !entries.empty() && selected < entries.size()-1) selected++;
        else if (key == 'q') return false;
        else if (key == 'd') {
            if (selected != -1 && !fs::is_directory(entries[selected])) {
                fs::remove(entries[selected]);
                if (selected == entries.size()-1) selected--;
            }
            else {
                printw("Empty / Is Directory");
                getch();
            }
        }
        else if (key == '\n' || key == KEY_ENTER) {
            if(selected == -1) {
                printw("Empty Directory");
                getch();
            }
            else if (fs::is_directory(entries[selected])) {
                current_path = entries[selected];
                selected = (fs::is_empty(current_path) ? -1 : 0);
                
            }
            else {
                printw("Not a Directory");
                getch();
            }
        }
        else if (key == KEY_BACKSPACE) {
            current_path = (current_path/"..").lexically_normal();
            selected = 0;
        }
        return true;;
    }
    
    void display(const std::vector<fs::path>& entries) {
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        scroll_offset = std::max(0, selected - rows + 10);
        mvprintw(0, 0, "Current Directory : %s", current_path.c_str());
        mvprintw(0, 50, "Selected : %d", selected);
        mvprintw(1, 0, "----------------------------------------------");
        for (int i = scroll_offset;i<std::min(scroll_offset + rows - 4, (int)entries.size());i++) {
            auto& entry = entries[i];
            mvprintw(2+i-scroll_offset, 0, "%s %s %s\n", i == selected ? ">" : " "
                               , fs::is_directory(entry) ? " [D] " : " [F] "
                               , entry.filename().c_str());
        }
        mvprintw(rows-1, 0, "^/v Navigate");
        mvprintw(rows-1, 15, "Enter Open");
        mvprintw(rows-1, 28, "BackSpace UP");
        mvprintw(rows-1, 43, "q Quit");
    }
};

#endif
