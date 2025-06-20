#include "checkout.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>

namespace fs = std::filesystem;

bool isCommitHash(const std::string& arg) {
    return arg.size() >= 8 && fs::exists(".minigit/objects/" + arg);
}

std::string readFile(const std::string& path) {
    std::ifstream in(path);
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    out << content;
}

std::string resolveCommitHash(const std::string& input) {
    if (isCommitHash(input)) return input;

    std::string path = ".minigit/refs/" + input;
    if (fs::exists(path)) return readFile(path);

    std::cerr << "Error: No such branch or commit: " << input << "\n";
    return "";
}

void clearWorkingDirectory() {
    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.path().filename() == ".minigit") continue;
        fs::remove_all(entry.path());
    }
}

void clearIndex() {
    std::ofstream(".minigit/index").close(); 
}

void restoreFilesFromCommit(const std::string& commitHash) {
    std::ifstream in(".minigit/objects/" + commitHash);
    if (!in) {
        std::cerr << "Error: Commit object not found: " << commitHash << "\n";
        return;
    }

    std::string line;
    while (std::getline(in, line)) {
        if (line.find(" ") == std::string::npos ||
            line.find("Message:") == 0 || line.find("Timestamp:") == 0 || 
            line.find("Parent") == 0) continue;

        std::istringstream iss(line);
        std::string filename, blobHash;
        iss >> filename >> blobHash;

        std::ifstream blob(".minigit/objects/" + blobHash);
        if (!blob) {
            std::cerr << "Missing blob: " << blobHash << " for file " << filename << "\n";
            continue;
        }

        std::ofstream out(filename);
        out << blob.rdbuf();
    }
}

void checkout(const std::string& target) {
    std::string commitHash = resolveCommitHash(target);
    if (commitHash.empty()) return;

    clearWorkingDirectory();
    restoreFilesFromCommit(commitHash);
    writeFile(".minigit/HEAD", commitHash);
    clearIndex();

    std::cout << "Checked out " << target << " (commit " << commitHash << ")\n";
}
