#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <filesystem>
#include <chrono>
#include <ctime>

namespace fs = std::filesystem;

std::string hashContent(const std::string& content) {
    std::hash<std::string> hasher;
    size_t hash = hasher(content);
    std::stringstream ss;
    ss << std::hex << hash;
    return ss.str();
}

void ensureRepo() {
    fs::create_directories(".minigit/objects");
    std::ofstream index(".minigit/index", std::ios::app);
    std::ofstream head(".minigit/HEAD", std::ios::app);
}

std::unordered_map<std::string, std::string> readIndex() {
    std::unordered_map<std::string, std::string> index;
    std::ifstream file(".minigit/index");
    std::string line;
    while (getline(file, line)) {
        auto sep = line.find(' ');
        if (sep != std::string::npos)
            index[line.substr(0, sep)] = line.substr(sep + 1);
    }
    return index;
}

void writeIndex(const std::unordered_map<std::string, std::string>& index) {
    std::ofstream file(".minigit/index");
    for (const auto& [fileName, hash] : index)
        file << fileName << " " << hash << "\n";
}

void add(const std::string& filename) {
    ensureRepo();
    if (!fs::exists(filename)) {
        std::cerr << "File " << filename << " does not exist.\n";
        return;
    }

    std::ifstream in(filename);
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    std::string blobHash = hashContent(content);

    std::ofstream out(".minigit/objects/" + blobHash);
    out << content;

    auto index = readIndex();
    index[filename] = blobHash;
    writeIndex(index);

    std::cout << "Staged " << filename << " as " << blobHash << "\n";
}

std::string getHead() {
    std::ifstream file(".minigit/HEAD");
    std::string head;
    getline(file, head);
    return head;
}

void commit(const std::string& message) {
    ensureRepo();
    auto index = readIndex();
    if (index.empty()) {
        std::cout << "Nothing to commit.\n";
        return;
    }

    std::stringstream commitContent;
    commitContent << "Message: " << message << "\n";
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    commitContent << "Timestamp: " << std::ctime(&now);
    commitContent << "Parent: " << getHead() << "\n";
    for (const auto& [file, hash] : index)
        commitContent << file << " " << hash << "\n";

    std::string contentStr = commitContent.str();
    std::string commitHash = hashContent(contentStr);

    std::ofstream out(".minigit/objects/" + commitHash);
    out << contentStr;

    std::ofstream head(".minigit/HEAD");
    head << commitHash;

    std::ofstream clear(".minigit/index");
    clear.close();

    std::cout << "Committed as " << commitHash << "\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: add <file> | commit -m <message>\n";
        return 1;
    }

    std::string cmd = argv[1];
    if (cmd == "add" && argc == 3) {
        add(argv[2]);
    } else if (cmd == "commit" && argc >= 4 && std::string(argv[2]) == "-m") {
        std::stringstream msg;
        for (int i = 3; i < argc; ++i)
            msg << argv[i] << " ";
        commit(msg.str());
    } else {
        std::cerr << "Invalid command.\n";
    }

    return 0;
}
