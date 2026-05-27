#include "../include/Professor.h"
#include "../include/IDgenerator.h"
#include "../include/LoginException.h"
#include <iostream>
#include <fstream>
using namespace std;

// ── string binary helpers ─────────────────────────────────────────────────
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

// ── constructors ──────────────────────────────────────────────────────────
Professor::Professor(string name, string email, string password, string dept)
    : User(IDgenerator::generateProfessorID(), name, email, password),
      department(dept) {
    cout << "[Professor] '" << name << "' (ID: " << userId
         << ") created. Dept: " << dept << endl;
}

// Load-from-file constructor: ID already known
Professor::Professor(string id, string name, string email,
                     string password, string dept)
    : User(id, name, email, password), department(dept) {}

Professor::~Professor() {
    cout << "[Professor] '" << name << "' (ID: " << userId << ") destroyed." << endl;
}

// ── getters ───────────────────────────────────────────────────────────────
string         Professor::getDepartment() const { return department;      }
vector<string> Professor::getCourseIds()  const { return taughtCourseIds; }

void Professor::assignCourse(string courseId) {
    for (int i = 0; i < (int)taughtCourseIds.size(); i++) {
        if (taughtCourseIds[i] == courseId) {
            cout << "[Professor] Already assigned to course " << courseId << endl;
            return;
        }
    }
    taughtCourseIds.push_back(courseId);
    cout << "[Professor] " << name << " assigned to course " << courseId << endl;
}

// ── binary file I/O ───────────────────────────────────────────────────────
void Professor::writeToBinaryFull(ofstream& out) const {
    User::writeToBinary(out);   // userId, name, email, password

    writeStr(out, department);

    int count = (int)taughtCourseIds.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++) {
        writeStr(out, taughtCourseIds[i]);
    }
}

void Professor::readFromBinaryFull(ifstream& in) {
    User::readFromBinary(in);

    readStr(in, department);

    int count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    taughtCourseIds.clear();
    for (int i = 0; i < count; i++) {
        string c; readStr(in, c);
        taughtCourseIds.push_back(c);
    }
}

void Professor::saveToFile() const {
    ofstream out("professors.dat", ios::binary | ios::app);
    if (!out.is_open()) {
        throw FileException("Cannot open professors.dat for writing.");
    }
    writeToBinaryFull(out);
    out.close();
    cout << "[Professor] Data appended to professors.dat" << endl;
}

// ── panel ─────────────────────────────────────────────────────────────────
string Professor::getRole() const { return "Professor"; }

void Professor::displayPanel() {
    int choice = 0;
    cout << "\n=== Professor Panel  |  " << name
         << "  |  ID: " << userId
         << "  |  Dept: " << department << " ===" << endl;
    cout << "  1. View my assigned courses" << endl;
    cout << "  0. Logout" << endl;
    cout << "Choice: ";
    cin >> choice;

    if (choice == 1) {
        cout << "\n--- Assigned Courses ---" << endl;
        if (taughtCourseIds.empty()) {
            cout << "  (none assigned yet)" << endl;
        }
        for (int i = 0; i < (int)taughtCourseIds.size(); i++) {
            cout << "  - " << taughtCourseIds[i] << endl;
        }
        displayPanel();
    }
    // choice 0 falls through — returns to main loop
}
