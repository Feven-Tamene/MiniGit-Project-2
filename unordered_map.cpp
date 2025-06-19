#include <unordered_map>
#include <iostream>

int main() {
    std::unordered_map<std::string, int> map;
    map["apple"] = 3;
    map["banana"] = 5;

    for (const auto& [key, value] : map) {
        std::cout << key << ": " << value << "\n";
    }
    return 0;
}
