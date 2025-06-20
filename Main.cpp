#include "checkout.h"
#include "merge.h"
#include <iostream>
#include <sstream>

void command() {
    std::string input;
    std::cout << "Welcome to MiniGit! Type 'exit' to quit.\n";

    while (true) {
        std::cout << "- ";
        std::getline(std::cin, input);
        std::istringstream iss(input);
        std::string cmd;
        iss >> cmd;

        if (cmd == "exit") {
            std::cout << "Exiting MiniGit.\n";
            break;
        }
        else if (cmd == "checkout") {
            std::string arg;
            iss >> arg;
            if (arg.empty()) {
                std::cerr << "Usage: checkout <branch|commit>\n";
            } else {
                checkout(arg);  
            }
        }

        else if (cmd == "merge") {
            std::string arg;
            iss >> arg;
            if (arg.empty()) {
                std::cerr << "Usage: merge <branch>\n";
            } else {
                merge(arg);  
            }
        }
    }
}
