#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main() {
    std::cout << "Files in current directory:\n";
    for (const auto& entry : fs::directory_iterator(".")) {
        std::cout << " - " << entry.path().filename() << "\n";
    }
    return 0;
}
