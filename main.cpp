#include "filemanager.h"

int main() {
    FileManager fm;
    while(true) {
        auto entries = fm.list();
        fm.display(entries);
        if (!fm.request(entries)) break;
        std::cout << "\n";
    }
    std::cout << "Exiting File Manager" << "\n";
    return 0;
}
