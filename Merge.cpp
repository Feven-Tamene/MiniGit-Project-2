#include <iostream>
#include <fstream>
#include <unordered_map>
#include <set>
#include <sstream>
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

std::string readFile(const std::string& path) {
    std::ifstream in(path);
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

std::unordered_map<std::string, std::string> readCommitFiles(const std::string& hash) {
    std::unordered_map<std::string, std::string> files;
    std::ifstream in(".minigit/objects/" + hash);
    std::string line;
    while (getline(in, line)) {
        if (line.find(" ") != std::string::npos && line.find("Message:") != 0 && line.find("Timestamp:") != 0 && line.find("Parent") != 0) {
            std::istringstream iss(line);
            std::string fname, blob;
            iss >> fname >> blob;
            files[fname] = blob;
        }
    }
    return files;
}

std::string getParent(const std::string& hash, int parentNum = 1) {
    std::ifstream in(".minigit/objects/" + hash);
    std::string line;
    std::string key = parentNum == 2 ? "Parent2:" : "Parent:";
    while (getline(in, line)) {
        if (line.find(key) == 0)
            return line.substr(key.length() + 1);
    }
    return "";
}

std::string getCurrentHEAD() {
    return readFile(".minigit/HEAD");
}

std::string findLCA(const std::string& a, const std::string& b) {
    std::set<std::string> ancestors;
    std::string curr = a;
    while (!curr.empty()) {
        ancestors.insert(curr);
        curr = getParent(curr);
    }
    curr = b;
    while (!curr.empty()) {
        if (ancestors.count(curr)) return curr;
        curr = getParent(curr);
    }
    return "";
}

std::string hashContent(const std::string& content) {
    std::hash<std::string> hasher;
    std::stringstream ss;
    ss << std::hex << hasher(content);
    return ss.str();
}

void writeBlob(const std::string& content, const std::string& blobHash) {
    std::ofstream out(".minigit/objects/" + blobHash);
    out << content;
}

void updateIndex(const std::unordered_map<std::string, std::string>& merged) {
    std::ofstream index(".minigit/index");
    for (const auto& [file, blob] : merged)
        index << file << " " << blob << "\n";
}

void merge(const std::string& branchName) {
    std::string targetCommit = readFile(".minigit/refs/" + branchName);
    std::string current = getCurrentHEAD();
    std::string base = findLCA(current, targetCommit);

    auto baseFiles = readCommitFiles(base);
    auto currFiles = readCommitFiles(current);
    auto otherFiles = readCommitFiles(targetCommit);

    std::set<std::string> allFiles;
    for (const auto& [f, _] : baseFiles) allFiles.insert(f);
    for (const auto& [f, _] : currFiles) allFiles.insert(f);
    for (const auto& [f, _] : otherFiles) allFiles.insert(f);

    std::unordered_map<std::string, std::string> merged;
    bool conflict = false;

    for (const auto& file : allFiles) {
        std::string baseBlob = baseFiles[file];
        std::string currBlob = currFiles[file];
        std::string otherBlob = otherFiles[file];

        if (currBlob == otherBlob || currBlob == baseBlob) {
            merged[file] = otherBlob;
        } else if (otherBlob == baseBlob) {
            merged[file] = currBlob;
        } else {
            std::string currContent = readFile(".minigit/objects/" + currBlob);
            std::string otherContent = readFile(".minigit/objects/" + otherBlob);
            std::ofstream out(file);
            out << "<<<<<<< HEAD\n" << currContent << "=======\n" << otherContent << ">>>>>>>\n";
            std::cout << "CONFLICT: " << file << "\n";
            conflict = true;
        }

        if (!merged[file].empty()) {
            std::string content = readFile(".minigit/objects/" + merged[file]);
            std::ofstream out(file);
            out << content;
        }
    }

    updateIndex(merged); 
    std::string msg = "Merge branch '" + branchName + "'";
    std::stringstream ss;
    ss << "Message: " << msg << "\n";
    auto now = std::time(nullptr);
    ss << "Timestamp: " << std::ctime(&now);
    ss << "Parent: " << current << "\n";
    ss << "Parent2: " << targetCommit << "\n";
    for (const auto& [file, blob] : merged) {
        ss << file << " " << blob << "\n";
    }

    std::string content = ss.str();
    std::string commitHash = hashContent(content);
    writeBlob(content, commitHash);

    std::ofstream head(".minigit/HEAD");
    head << commitHash;

    std::cout << (conflict ? "Merge completed with conflicts.\n" : "Merge completed successfully.\n");
}
