#ifndef FILEMANAGER_H
#define FILEMANAGER_H

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
    bool isValidDirectory(const int idx, const std::vector<fs::path>& entries) {
        return fs::is_directory(entries[idx]);
    }

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
        else if ((key == '\n' || key == KEY_ENTER) && !entries.empty()) {
            if (fs::is_directory(entries[selected]) && !fs::is_empty(entries[selected])) {
                current_path = entries[selected];
                selected = 0;
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
        printw("Current Directory : %s\n", current_path.c_str());
        printw("Selected : %d\n\n", selected);
        int idx = 0;
        for (auto& entry : entries) {
            printw("%s %s %s\n", idx == selected ? ">" : " "
                               , fs::is_directory(entry) ? " [D] " : " [F] "
                               , entry.filename().c_str());
            idx++;
        }
    }
};

#endif
