#include "../include/User.h"
using namespace std;

int User::totalUsers = 0;

// ── helpers for length-prefixed string binary I/O ─────────────────────────
static void writeStr(ofstream& out, const string& s) {
    int len = (int)s.size();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(s.c_str(), len);
}
static void readStr(ifstream& in, string& s) {
    int len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    s.resize(len);
    in.read(&s[0], len);
}

User::User(string id, string name, string email, string password)
    : userId(id), name(name), email(email), password(password) {
    totalUsers++;
    cout << "[User] '" << name << "' (ID: " << userId
         << ") created. Total users: " << totalUsers << endl;
}

User::~User() {
    totalUsers--;
}

string User::getUserId() const { return userId; }
string User::getName()   const { return name;   }
string User::getEmail()  const { return email;  }

bool User::checkPassword(string input) const {
    return password == input;
}

bool User::operator==(const User& other) const {
    return userId == other.userId;
}

ostream& operator<<(ostream& out, const User& u) {
    out << "[" << u.getRole() << "] "
        << u.name << " | ID: " << u.userId
        << " | Email: " << u.email;
    return out;
}

int User::getTotalUsers() {
    return totalUsers;
}

// Writes the 4 common fields; subclasses call this then append their own
void User::writeToBinary(ofstream& out) const {
    writeStr(out, userId);
    writeStr(out, name);
    writeStr(out, email);
    writeStr(out, password);
}

void User::readFromBinary(ifstream& in) {
    readStr(in, userId);
    readStr(in, name);
    readStr(in, email);
    readStr(in, password);
}
