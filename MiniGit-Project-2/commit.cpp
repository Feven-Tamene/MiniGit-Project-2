#include "commit.h"
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

Commit::Commit(const string& msg, const string& h, Commit* p)
    : message(msg), hash(h), parent(p) {
    timestamp = getCurrentTimestamp();
}

string getCurrentTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    ostringstream oss;
    oss << 1900 + ltm->tm_year << "-"
        << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
        << setw(2) << setfill('0') << ltm->tm_mday << " "
        << setw(2) << setfill('0') << ltm->tm_hour << ":"
        << setw(2) << setfill('0') << ltm->tm_min << ":"
        << setw(2) << setfill('0') << ltm->tm_sec;
    return oss.str();
}
