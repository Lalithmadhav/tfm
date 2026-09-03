#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class FileManager {
private:
    fs::path current_path;
    bool isValid(const int idx, const int esize) {
        return idx >= 0 && idx < esize;
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
        std::cout << "Enter q to quit or a number: ";
        std::string s;
        if (!getline(std::cin >> std::ws, s)) return false;
        if (s == "q") return false;
        if (s == "..") {
            current_path = (current_path/s).lexically_normal();
            return true;
        }
        int idx = std::stoi(s);
        if (!isValid(idx, entries.size())) {
            std::cout << "Not Valid" << "\n";
        } else current_path = entries[idx];
        return true;
    }
    
    void display(const std::vector<fs::path>& entries) {
        std::cout << "Current Directory : " << current_path<< "\n\n";
        int idx = 0;
        for (auto& entry : entries) {
            std::cout << idx++ << (fs::is_directory(entry) ? " [D] " : " [F] ")<< entry.filename() << "\n";
        }
    }
};

#endif
