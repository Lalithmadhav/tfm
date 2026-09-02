#include "filemanager.h"

int main() {
    FileManager fm;
    while(true) {
        fm.display();
        if (!fm.request()) break;
        std::cout << "\n";
    }
    std::cout << "Exiting File Manager" << "\n";
    return 0;
}
