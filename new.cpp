#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <string>
#include <set>

using namespace std;
namespace fs = std::filesystem;

string hashContent(const string& content) {
    hash<string> hasher;
    size_t hashVal = hasher(content);
    stringstream ss;
    ss << hex << hashVal;
    return ss.str();
}

void initialize() {
    if (!fs::exists(".minigit")) {
        fs::create_directory(".minigit");
        fs::create_directory(".minigit/objects");
        fs::create_directory(".minigit/refs");
        ofstream head(".minigit/HEAD");
        head << "ref: refs/main" << endl;
        ofstream mainRef(".minigit/refs/main");
        mainRef << "" << endl;
        ofstream index(".minigit/index");
        cout << "Initialized empty MiniGit repository!" << endl;
    } else {
        cout << "MiniGit repository already exists." << endl;
    }
}

unordered_map<string, string> readIndex() {
    unordered_map<string, string> index;
    ifstream file(".minigit/index");
    string line;
    while (getline(file, line)) {
        auto sep = line.find(' ');
        if (sep != string::npos)
            index[line.substr(0, sep)] = line.substr(sep + 1);
    }
    return index;
}

void writeIndex(const unordered_map<string, string>& index) {
    ofstream file(".minigit/index");
    for (const auto& [fileName, hash] : index)
        file << fileName << " " << hash << "\n";
}

void add(const string& filename) {
    if (!fs::exists(filename)) {
        cerr << "File " << filename << " does not exist.\n";
        return;
    }
    ifstream in(filename);
    stringstream buffer;
    buffer << in.rdbuf();
    string content = buffer.str();

    string blobHash = hashContent(content);
    ofstream out(".minigit/objects/" + blobHash);
    out << content;

    auto index = readIndex();
    index[filename] = blobHash;
    writeIndex(index);

    cout << "Staged " << filename << " as " << blobHash << "\n";
}

string getHead() {
    ifstream file(".minigit/HEAD");
    string head;
    getline(file, head);
    if (head.rfind("ref:", 0) == 0) {
        string ref = head.substr(5);
        ifstream refFile(".minigit/" + ref);
        string refHash;
        getline(refFile, refHash);
        return refHash;
    }
    return head;
}

void commit(const string& message) {
    auto index = readIndex();
    if (index.empty()) {
        cout << "Nothing to commit.\n";
        return;
    }

    stringstream commitContent;
    commitContent << "Message: " << message << "\n";
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    commitContent << "Timestamp: " << ctime(&now);
    commitContent << "Parent: " << getHead() << "\n";

    for (const auto& [file, hash] : index)
        commitContent << file << " " << hash << "\n";

    string contentStr = commitContent.str();
    string commitHash = hashContent(contentStr);

    ofstream out(".minigit/objects/" + commitHash);
    out << contentStr;

    ifstream headFile(".minigit/HEAD");
    string headLine;
    getline(headFile, headLine);
    if (headLine.rfind("ref:", 0) == 0) {
        string ref = headLine.substr(5);
        ofstream refFile(".minigit/" + ref);
        refFile << commitHash << endl;
    }
    ofstream headOut(".minigit/HEAD");
    headOut << commitHash << endl;

    ofstream clearIndex(".minigit/index");
    clearIndex.close();

    cout << "Committed as " << commitHash << endl;
}

void printLog(const string& headHash) {
    string current = headHash;
    while (!current.empty()) {
        ifstream in(".minigit/objects/" + current);
        if (!in) break;

        string line, message, timestamp, parent;
        while (getline(in, line)) {
            if (line.rfind("Message:", 0) == 0)
                message = line.substr(8);
            else if (line.rfind("Timestamp:", 0) == 0)
                timestamp = line.substr(10);
            else if (line.rfind("Parent:", 0) == 0)
                parent = line.substr(7);
            if (line.empty()) break;
        }

        cout << "Commit: " << current << "\nMessage:" << message << "\nTime:" << timestamp << endl;
        cout << "----------------------\n";

        current = parent;
    }
}

void createBranch(const string& branchName) {
    string headHash = getHead();
    fs::create_directories(".minigit/refs");
    ofstream out(".minigit/refs/" + branchName);
    out << headHash << endl;
    cout << "Branch '" << branchName << "' created at commit " << headHash << endl;
}

void checkout(const string& target) {
    string commitHash;
    string branchPath = ".minigit/refs/" + target;
    if (fs::exists(branchPath)) {
        ifstream branchFile(branchPath);
        if (!branchFile) {
            cerr << "Failed to open branch file.\n";
            return;
        }
        getline(branchFile, commitHash);
    } else if (fs::exists(".minigit/objects/" + target)) {
        commitHash = target;
    } else {
        cerr << "No such branch or commit: " << target << endl;
        return;
    }

    if (commitHash.empty()) {
        cerr << "Commit hash is empty — aborting.\n";
        return;
    }

    ifstream commitFile(".minigit/objects/" + commitHash);
    if (!commitFile) {
        cerr << "Commit file not found for hash: " << commitHash << endl;
        return;
    }

    for (auto& p : fs::directory_iterator(".")) {
    string name = p.path().filename().string();
    if (name == ".minigit" || name == ".vscode" || name == "node_modules") 
        continue;  // skip these folders

    try {
        fs::remove_all(p.path());
    } catch (const fs::filesystem_error& e) {
        cerr << "Warning: Could not remove " << p.path() << ": " << e.what() << endl;
    }
}

    string line;
    while (getline(commitFile, line)) {
        if (line.find(" ") == string::npos ||
            line.find("Message:") == 0 ||
            line.find("Timestamp:") == 0 ||
            line.find("Parent:") == 0) continue;

        istringstream iss(line);
        string fname, blobHash;
        iss >> fname >> blobHash;

        ifstream blobFile(".minigit/objects/" + blobHash);
        if (!blobFile) {
            cerr << "Missing blob: " << blobHash << " for file " << fname << "\n";
            continue;
        }

        ofstream out(fname);
        out << blobFile.rdbuf();
    }

    ofstream head(".minigit/HEAD");
    head << commitHash << endl;

    ofstream(".minigit/index").close();

    cout << "Checked out " << target << " (commit " << commitHash << ")\n";
}

void command() {
    string input;
    cout << "Welcome to MiniGit! Type 'exit' to quit.\n";

    while (true) {
        cout << "- ";
        getline(cin, input);
        istringstream iss(input);
        string cmd;
        iss >> cmd;

        if (cmd == "exit") {
            cout << "Exiting MiniGit.\n";
            break;
        } else if (cmd == "init") {
            initialize();
        } else if (cmd == "add") {
            string filename;
            iss >> filename;
            if (filename.empty()) cout << "Usage: add <filename>\n";
            else add(filename);
        } else if (cmd == "commit") {
            string flag;
            iss >> flag;
            if (flag != "-m") {
                cout << "Usage: commit -m <message>\n";
                continue;
            }
            string message;
            getline(iss, message);
            if (message.empty()) {
                cout << "Commit message cannot be empty.\n";
                continue;
            }
            if (message[0] == ' ') message.erase(0, 1);
            commit(message);
        } else if (cmd == "log") {
            string headHash = getHead();
            if (headHash.empty()) cout << "No commits yet.\n";
            else printLog(headHash);
        } else if (cmd == "branch") {
            string branchName;
            iss >> branchName;
            if (branchName.empty()) cout << "Usage: branch <branch-name>\n";
            else createBranch(branchName);
        } else if (cmd == "checkout") {
            string target;
            iss >> target;
            if (target.empty()) cout << "Usage: checkout <branch|commit>\n";
            else checkout(target);
        } else {
            cout << "Unknown command. Available: init, add, commit, log, branch, checkout, exit\n";
        }
    }
}

int main() {
    command();
    return 0;
}
