#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace std;
namespace fs = std::filesystem; 

// Function to initialize the repository
void initialize () {
    if (!fs::exists(".minigit")) {
        fs::create_directory(".minigit");
        fs::create_directory(".minigit/objects");
        fs::create_directory(".minigit/refs");
        ofstream head(".minigit/HEAD");
        head << "ref: refs/main" << endl;
        head.close();
        ofstream mainRef(".minigit/refs/main");
        mainRef << "" << endl;
        cout << "Initialized empty MiniGit repository!" << endl;
    } else {
        cout << "MiniGit repository already exists." << endl;
    }
}

// Main command 
void command() {
    string input;
    cout << "Welcome to MiniGit! Type 'exit' to quit.\n";

while (true){
        cout << "- ";
        getline(cin, input);
        istringstream iss(input);
        string command;
        iss >> command;

        if (command == "init") {
            initialize();
        } else if (command == "exit") {
            cout << "Exiting MiniGit.\n";
            break;
        } else {
            cout << " Try again Please.\n";
        }
    }
}

int main() {
    command();
    return 0;
 }
