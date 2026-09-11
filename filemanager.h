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
    int rows = -1, cols = -1;

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
                mvprintw(rows - 2, 0, "Empty / Is Directory");
                getch();
            }
        }
        else if (key == '\n' || key == KEY_ENTER) {
            if(selected == -1) {
                mvprintw(rows - 2, 0,"Empty Directory");
                getch();
            }
            else if (fs::is_directory(entries[selected])) {
                current_path = entries[selected];
                selected = (fs::is_empty(current_path) ? -1 : 0);
                
            }
            else {
                mvprintw(rows - 2, 0, "Not a Directory");
                getch();
            }
        }
        else if (key == KEY_BACKSPACE) {
            current_path = (current_path/"..").lexically_normal();
            selected = 0;
        }
        else if (key == 'r') {
            if (selected == -1) {
                mvprintw(rows - 2, 0, "Invalid");
                return true;
            }
            fs::path oldp = entries[selected];
            char newp[20];

            echo();
            mvprintw(rows-2, 0, "New name : ");
            getnstr(newp, 20);
            noecho();
            for (auto i : entries) {
                if (i.filename() == newp) {
                    mvprintw(rows-2, 0, "File already exists");
                    getch();
                    return true;
                }
            }
            try {
                fs::rename(oldp, current_path/newp);
            } catch (fs::filesystem_error& e) {
                mvprintw(rows-2, 0, "Invalid : %s", e.what());
                getch();
            }
        }
        return true;;
    }
    
    void display(const std::vector<fs::path>& entries) {
        getmaxyx(stdscr, rows, cols);
        scroll_offset = std::max(0, selected - rows + 10);
        mvprintw(0, 0, "Current Directory : %s", current_path.c_str());
        mvprintw(0, cols*3/4, "Selected : %d", selected);
        mvprintw(1, 0, "----------------------------------------------");
        for (int i = scroll_offset;i<std::min(scroll_offset + rows - 4, (int)entries.size());i++) {
            auto& entry = entries[i];
            mvprintw(2+i-scroll_offset, 0, "%s %s %s\n", i == selected ? ">" : " "
                               , fs::is_directory(entry) ? " [D] " : " [F] "
                               , entry.filename().c_str());
        }
        mvprintw(rows-1, 0, "^/v Navigate");
        mvprintw(rows-1, cols/4, "Enter Open");
        mvprintw(rows-1, cols/2 , "BackSpace UP");
        mvprintw(rows-1, cols*3/4, "q Quit");
    }
};

#endif
