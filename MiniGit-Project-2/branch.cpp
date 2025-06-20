#include "branch.h"
#include <fstream>
#include <filesystem>

using namespace std;

void createBranch(const string& branchName, const string& currentCommitHash) {
    filesystem::create_directories(".minigit/refs");
    ofstream out(".minigit/refs/" + branchName + ".txt");
    out << currentCommitHash;
    out.close();
}