#include "log.h"
#include <iostream>

using namespace std;

void printLog(Commit* head) {
    Commit* current = head;
    while (current != nullptr) {
        cout << "Commit: " << current->hash << endl;
        cout << "Message: " << current->message << endl;
        cout << "Time: " << current->timestamp << endl;
        cout << endl;
        current = current->parent;
    }
}