#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class FileManager {
private:
    fs::path current_path;
    bool isValid(const std::string& s) {
        fs::path dpath = current_path/s;
        return fs::exists(dpath) && fs::is_directory(dpath);
    }

    void list() {
        for (const auto& dir_entry : fs::directory_iterator(current_path)) {
            std::cout << (fs::is_directory(dir_entry) ? "[D]" : "[F]")
                      << " " 
                      << dir_entry.path().filename()
                      << "\n";
        }
    }

public: 
    FileManager() : current_path(fs::current_path()) {};
    bool request() {
        std::cout << "Enter q to quit or a directory : ";
        std::string s;
        if (!getline(std::cin >> std::ws, s)) return false;
        if (s == "q") return false;
        if (!isValid(s)) {
            std::cout << "Not Valid" << "\n";
        } else current_path = (current_path/s).lexically_normal();
        return true;
    }
    
    void display() {
        std::cout << "Current Directory : " << current_path<< "\n\n";
        list();
    }
};

#endif
