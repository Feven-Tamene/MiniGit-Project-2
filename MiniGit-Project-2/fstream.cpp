#include <fstream>
#include <iostream>

int main() {
    std::ofstream out("fstream_output.txt");
    out << "This file was written using std::ofstream.\n";
    out.close();

    std::ifstream in("fstream_output.txt");
    std::string line;
    while (std::getline(in, line)) {
        std::cout << line << "\n";
    }
    return 0;
}
