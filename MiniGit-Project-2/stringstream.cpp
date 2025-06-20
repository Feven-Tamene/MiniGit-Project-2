#include <sstream>
#include <iostream>

int main() {
    std::stringstream ss;
    ss << "Using stringstream to build a string: ";
    int a = 42;
    ss << "value = " << a;

    std::string result = ss.str();
    std::cout << result << "\n";
    return 0;
}
