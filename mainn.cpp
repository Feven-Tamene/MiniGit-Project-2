#include <iostream>
#include <string>
#include "commit.h"
#include "log.h"
#include "branch.h"

using namespace std;

int main() {
    
    Commit* c1 = new Commit("Initial commit", "a1b2c3", nullptr);
    Commit* c2 = new Commit("Added README", "d4e5f6", c1);
    Commit* c3 = new Commit("Bug fix", "g7h8i9", c2);

    string command;
    while (true) {
        cout << "$ minigit ";
        cin >> command;

        if (command == "log") {
            printLog(c3); 
        } else if (command == "branch") {
            string branchName;
            cin >> branchName;
            createBranch(branchName, c3->hash);
            cout << "Branch '" << branchName << "' created at commit " << c3->hash << endl;
        } else if (command == "exit") {
            break;
        } else {
            cout << "Unknown command. Try: log, branch <name>, exit" << endl;
        }
    }

    delete c1;
    delete c2;
    delete c3;

    return 0;
}
