#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

bool isValid(string& dname, fs::path& cp) {
    for (auto const& dir_entry : fs::directory_iterator(cp)) {
        if (dname == dir_entry.path().filename()) {
            if (fs::is_directory(dir_entry.path())) return true;
            else {
                cout << "Not a directory" << "\n";
                return false;
            }
        }
    }
    cout << "Directory not found" << "\n";
    return false;

}
void list(const fs::path& path) {
    for (auto const& dir_entry : fs::directory_iterator(path)) {
        cout << (fs::is_directory(dir_entry.path()) ? "[DIR] " : "[FILE]") << " " << dir_entry.path().filename() << "\n";
    }
}

int main() {
    fs::path cp = fs::current_path();
    while (true) {
        cout << "Current Directory : " << cp << "\n";
        cout << "\n";
        list(cp);
        
        cout << "Enter q to quit or a Directory : ";
        string cmd;
        cin >> cmd;
        if (cmd == "q") break;
        if (cmd != ".." && !isValid(cmd, cp)) {
            break;
        }
        cp = (cp/cmd).lexically_normal();
    }
    return 0;
}
