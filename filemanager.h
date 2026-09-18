#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <algorithm>
#include <cstdio>
#include <fstream>
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

    int findPathIndex(const fs::path& toFindPath) {
        auto entries = list();
        for (int i = 0;i<entries.size();i++) if (entries[i] == toFindPath) return i;
        return -1;
    }

    void goToAndClearLine(const int y, const int x) {
        move(y, x);
        clrtoeol();
    }

    void showMessage(const char* msg) {
        goToAndClearLine(rows-2, 0);
        printw("%s", msg);
        getch();
    }

    void preview(const fs::path& path) {
        std::ifstream file(path);

        std::string line;
        std::vector<std::string> lines; 

        while (std::getline(file, line)) {
            lines.push_back(line);
        }
   
        int visible_rows = rows-2;
        if (lines.empty()) {
            mvprintw(visible_rows, 0, "Empty File");
            getch();
            return;
        }

        int preview_scroll_offset = 0, preview_line = 0;
        
        while (true) {
            clear();
           for (int i = preview_scroll_offset;i < std::min((int)lines.size(), visible_rows + preview_scroll_offset);i++) {
                mvprintw(i-preview_scroll_offset, 2,"%d %s",i,  lines[i].c_str());
            }

            refresh();
            int key = getch();
            if (key == KEY_DOWN && preview_line < lines.size()-1) preview_line++;
            else if (key == KEY_UP && preview_line > 0) preview_line--;
            else if (key == 'q') return;
            preview_scroll_offset = std::max(0, preview_line - visible_rows + 5);
        }
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
        else if (key == 'd') {
            if (selected != -1 && !fs::is_directory(entries[selected])) {
                fs::remove(entries[selected]);
                if (selected == entries.size()-1) selected--;
            }
            else {
                showMessage("Empty / Is Directory");
            }
        }
        else if (key == '\n' || key == KEY_ENTER) {
            if(selected == -1) {
                showMessage("Empty Directory");
            }
            else if (fs::is_directory(entries[selected])) {
                current_path = entries[selected];
                selected = (fs::is_empty(current_path) ? -1 : 0);
                
            }
            else {
                preview(entries[selected]);
            }
        }
        else if (key == KEY_BACKSPACE) {
            current_path = (current_path/"..").lexically_normal();
            selected = 0;
        }
        else if (key == 'r') {
            if (selected == -1) {
                showMessage("Invalid");
                return true;
            }
            fs::path oldp = entries[selected];
            char newp[20];

            echo();
            goToAndClearLine(rows-2, 0);
            printw("New name : ");
            getnstr(newp, 20);
            noecho();
            if (newp[0] == '\0') {
                showMessage("Cannot rename to empty");
                return 1;
            }

            for (auto i : entries) {
                if (i.filename() == newp) {
                    showMessage("File already exists");
                    return true;
                }
            }
            try {
                fs::rename(oldp, current_path/newp);
            } catch (const fs::filesystem_error& e) {
                goToAndClearLine(rows-2, 0);
                printw("Invalid : %s", e.what());
                getch();
            }
        }
        else if (key == 'm') {
            char dirName[20];
            goToAndClearLine(rows-2, 0);
            printw("Directory Name : ");
            echo();
            getnstr(dirName, 19);
            noecho();
            if (dirName[0] == '\0') {
                showMessage("Cannot name empty");
                return true;
            } 
            fs::path newPath = current_path/dirName;
            if (fs::exists(newPath)) {
                showMessage("Something already exists in this path.");
                return 1;
            }
            try {
                fs::create_directory(newPath);
                selected = findPathIndex(newPath);
            } catch (const fs::filesystem_error& e) {
                goToAndClearLine(rows-2, 0);
                printw("Invalid : %s", e.what());
                getch();
            }
        }
        else if (key == 'n') {
            char fName[20];
            goToAndClearLine(rows-2, 0);
            printw("File Name : ");
            echo();
            getnstr(fName, 19);
            noecho();
            if (fName[0] == '\0') {
                showMessage("Cannot name empty");
                return true;
            } 
            fs::path newPath = current_path/fName;
            if (fs::exists(newPath)) {
                showMessage("Something already exists in this path.");
                return 1;
            }
            std::ofstream file(newPath);
            if (!file) {
                showMessage("File not created.");
            } else {
                selected = findPathIndex(newPath);
                showMessage("File created. ");
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
