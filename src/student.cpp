#include "../include/Student.h"
#include "../include/IDgenerator.h"
#include "../include/LoginException.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
using namespace std;

// ── string binary helpers (duplicated locally to avoid a shared header) ───
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
// Normal creation: let IDgenerator make the ID
Student::Student(string name, string email, string password)
    : User(IDgenerator::generateStudentID(), name, email, password), gpa(0.0) {}

// Load-from-file creation: ID already known
Student::Student(string id, string name, string email, string password)
    : User(id, name, email, password), gpa(0.0) {}

Student::~Student() {
    cout << "[Student] '" << name << "' (ID: " << userId << ") destroyed." << endl;
}

// ── getters ───────────────────────────────────────────────────────────────
vector<string> Student::getCourses() const { return enrolledCourses; }
vector<double> Student::getGrades()  const { return grades; }
double         Student::getGPA()     const { return gpa; }

// ── enrolment & grades ────────────────────────────────────────────────────
void Student::enrollCourse(string courseId) {
    // check for duplicate
    for (int i = 0; i < (int)enrolledCourses.size(); i++) {
        if (enrolledCourses[i] == courseId) {
            cout << "[Student] Already enrolled in course " << courseId << endl;
            return;
        }
    }
    enrolledCourses.push_back(courseId);
    grades.push_back(-1.0);   // -1 = no grade yet
    cout << "[Student] " << name << " enrolled in course " << courseId << endl;
}

void Student::addGrade(double score) {
    // Validation: score must be 0-100
    if (score < 0.0 || score > 100.0) {
        throw ValidationException(
            "Score " + to_string(score) + " is out of range (0-100).");
    }
    grades.push_back(score);
    recalcGPA();
}

void Student::recalcGPA() {
    int count = 0;
    double total = 0.0;
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i] >= 0.0) {
            total += grades[i];
            count++;
        }
    }
    gpa = (count > 0) ? total / count : 0.0;

    // Validation: GPA must stay in 0-100 range
    if (gpa < 0.0 || gpa > 100.0) {
        throw ValidationException("Calculated GPA is out of valid range.");
    }
}

// ── binary file I/O ───────────────────────────────────────────────────────
void Student::writeToBinaryFull(ofstream& out) const {
    User::writeToBinary(out);   // userId, name, email, password

    // GPA
    out.write(reinterpret_cast<const char*>(&gpa), sizeof(gpa));

    // enrolled courses
    int courseCount = (int)enrolledCourses.size();
    out.write(reinterpret_cast<const char*>(&courseCount), sizeof(courseCount));
    for (int i = 0; i < courseCount; i++) {
        writeStr(out, enrolledCourses[i]);
    }

    // grades (parallel to courses)
    int gradeCount = (int)grades.size();
    out.write(reinterpret_cast<const char*>(&gradeCount), sizeof(gradeCount));
    for (int i = 0; i < gradeCount; i++) {
        out.write(reinterpret_cast<const char*>(&grades[i]), sizeof(double));
    }
}

void Student::readFromBinaryFull(ifstream& in) {
    User::readFromBinary(in);

    in.read(reinterpret_cast<char*>(&gpa), sizeof(gpa));

    int courseCount = 0;
    in.read(reinterpret_cast<char*>(&courseCount), sizeof(courseCount));
    enrolledCourses.clear();
    for (int i = 0; i < courseCount; i++) {
        string c; readStr(in, c);
        enrolledCourses.push_back(c);
    }

    int gradeCount = 0;
    in.read(reinterpret_cast<char*>(&gradeCount), sizeof(gradeCount));
    grades.clear();
    for (int i = 0; i < gradeCount; i++) {
        double g = 0.0;
        in.read(reinterpret_cast<char*>(&g), sizeof(g));
        grades.push_back(g);
    }
}

// Saves this one student (append) — used internally; Admin saves all
void Student::saveToFile() const {
    ofstream out("students.dat", ios::binary | ios::app);
    if (!out.is_open()) {
        throw FileException("Cannot open students.dat for writing.");
    }
    writeToBinaryFull(out);
    out.close();
    cout << "[Student] Data appended to students.dat" << endl;
}

// ── panel ─────────────────────────────────────────────────────────────────
string Student::getRole() const { return "Student"; }

void Student::displayPanel() {
    int choice = 0;
    cout << "\n=== Student Panel  |  " << name
         << "  |  ID: " << userId << " ===" << endl;
    cout << "  1. View my enrolled courses & grades" << endl;
    cout << "  2. View my GPA" << endl;
    cout << "  0. Logout" << endl;
    cout << "Choice: ";
    cin >> choice;

    if (choice == 1) {
        cout << "\n--- Enrolled Courses ---" << endl;
        if (enrolledCourses.empty()) {
            cout << "  (none yet)" << endl;
        }
        for (int i = 0; i < (int)enrolledCourses.size(); i++) {
            cout << "  Course ID: " << enrolledCourses[i] << "  |  Grade: ";
            if (grades[i] < 0.0) cout << "N/A" << endl;
            else cout << fixed << setprecision(1) << grades[i] << endl;
        }
        displayPanel();
    } else if (choice == 2) {
        cout << fixed << setprecision(2);
        cout << "\n  Score: " << gpa << endl;
        displayPanel();
    }
    // choice 0 falls through — returns to main loop
}
