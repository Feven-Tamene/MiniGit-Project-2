#ifndef COMMIT_H
#define COMMIT_H

#include <string>
#include <vector>
#include <ctime>

using namespace std;

struct Commit {
    string hash;
    string message;
    string timestamp;
    Commit* parent;

    Commit(const string& msg, const string& h, Commit* p);
};

string getCurrentTimestamp();

#endif