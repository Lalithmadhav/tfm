#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

void list() {
    for (auto const& dir_entry : fs::directory_iterator(fs::current_path())) {
        cout << (fs::is_directory(dir_entry.path()) ? "[DIR]    " : "[NOT DIR]") << " " << dir_entry.path().filename() << "\n";
    }
}

int main() {
    cout << "Current Directory : " << fs::current_path() << "\n";
    list();
    return 0;
}
