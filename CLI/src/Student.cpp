#include "../include/student.h"
#include "../include/admin.h"
#include "../include/IDgenerator.h"
#include "../include/LoginException.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
using namespace std;

//clear window
static void clearScreen(){
    // ANSI escape clears the window of terminal
    cout << "\033[2J\033[H";
}

// ── string binary helper
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

// ── constructors
//let IDgenerator make the ID
Student::Student(string name, string email, string password)
    : User(IDgenerator::generateStudentID(), name, email, password), gpa(0.0),
      annualTuition(5000.0), semesterTuition(2500.0), tuitionPaid(0.0) {}

// Load-from-file creation: ID already known
Student::Student(string id, string name, string email, string password) : User(id, name, email, password), gpa(0.0),
      annualTuition(5000.0), semesterTuition(2500.0), tuitionPaid(0.0) {}

Student::~Student() {
    cout << "[Student] '" << name << "' (ID: " << userId << ") destroyed." << endl;
}

// ── getters
vector<string> Student::getCourses() const {
    return enrolledCourses;
}
vector<double> Student::getGrades() const { 
    return grades;
}
double Student::getGPA() const {
    return gpa;
}

// ── enrolment & grades
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
        throw ValidationException("Score " + to_string(score) + " is out of range (0-100).");
    }
    grades.push_back(score);
    recalcGPA();
}

void Student::setGrade(string courseId, double score) {
    bool found = false;
    for (int i = 0; i < (int)enrolledCourses.size(); i++) {
        if (enrolledCourses[i] == courseId) {
            grades[i] = score;
            found = true;
            break;
        }
    }
    if (!found) {
        throw ValidationException("Student is not enrolled in course " + courseId);
    }
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

// ── New Interfaces

void Student::viewTimetable(const Admin& admin) const {
    cout << "\n=== My Timetable: " << name << " ===" << endl;
    if (enrolledCourses.empty()) {
        cout << "  (no courses enrolled)" << endl;
        return;
    }

    const vector<Course>& allCourses = admin.getCourses();
    cout << left << setw(10) << "ID" << setw(20) << "Course Name" << setw(15) << "Days" << "Time Slot" << endl;
    cout << string(60, '-') << endl;

    for (const string& cid : enrolledCourses) {
        bool found = false;
        for (const Course& c : allCourses) {
            if (c.courseId == cid) {
                cout << left << setw(10) << c.courseId 
                     << setw(20) << c.courseName 
                     << setw(15) << c.daysOfWeek 
                     << c.timeSlot << endl;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << left << setw(10) << cid << " (Course details not found in system)" << endl;
        }
    }
}

void Student::viewTuitionStatus() const {
    double balance = semesterTuition - tuitionPaid;
    cout << "\n=== Tuition Balance System ===" << endl;
    cout << "  Annual Tuition   : $" << fixed << setprecision(2) << annualTuition << endl;
    cout << "  Semester Tuition : $" << semesterTuition << endl;
    cout << "  Tuition Paid     : $" << tuitionPaid << endl;
    cout << "  ------------------------------" << endl;
    cout << "  Outstanding Debt : $" << (balance > 0 ? balance : 0.0) << endl;
    if (balance <= 0) cout << "  Status           : PAID IN FULL" << endl;
    else cout << "  Status           : OVERDUE" << endl;
}

void Student::printTranscript() const {
    cout << "\n" << string(40, '=') << endl;
    cout << "       OFFICIAL ACADEMIC TRANSCRIPT" << endl;
    cout << string(40, '=') << endl;
    cout << "  Student Name: " << name << endl;
    cout << "  Student ID  : " << userId << endl;
    cout << string(40, '-') << endl;
    cout << left << setw(12) << "Course ID" << "Grade" << endl;
    cout << string(40, '-') << endl;

    for (int i = 0; i < (int)enrolledCourses.size(); i++) {
        cout << left << setw(12) << enrolledCourses[i];
        if (grades[i] < 0) cout << "N/A" << endl;
        else cout << fixed << setprecision(1) << grades[i] << endl;
    }

    cout << string(40, '-') << endl;
    cout << "  Cumulative GPA: " << fixed << setprecision(2) << gpa << endl;
    cout << string(40, '=') << endl;
}

// ── binary file I/O
void Student::writeToBinaryFull(ofstream& out) const {
    User::writeToBinary(out);   // userId, name, email, password

    // GPA and Tuition
    out.write(reinterpret_cast<const char*>(&gpa), sizeof(gpa));
    out.write(reinterpret_cast<const char*>(&annualTuition), sizeof(annualTuition));
    out.write(reinterpret_cast<const char*>(&semesterTuition), sizeof(semesterTuition));
    out.write(reinterpret_cast<const char*>(&tuitionPaid), sizeof(tuitionPaid));

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
    in.read(reinterpret_cast<char*>(&annualTuition), sizeof(annualTuition));
    in.read(reinterpret_cast<char*>(&semesterTuition), sizeof(semesterTuition));
    in.read(reinterpret_cast<char*>(&tuitionPaid), sizeof(tuitionPaid));

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
string Student::getRole() const { 
    return "Student";
}

void Student::displayPanel() {
    cout << "[Error] Student panel requires Admin context." << endl;
}

void Student::displayPanelWithAdmin(const Admin& admin) {
    int choice = -1;
    while (choice != 0) {
        clearScreen();
        cout << "\n=== Student Panel  |  " << name
             << "  |  ID: " << userId << " ===" << endl;
        cout << "  1. View my enrolled courses & grades" << endl;
        cout << "  2. View my GPA" << endl;
        cout << "  3. View my Timetable" << endl;
        cout << "  4. View Tuition Status" << endl;
        cout << "  5. Print Academic Transcript" << endl;
        cout << "  0. Logout" << endl;
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            cout << "\n--- Enrolled Courses ---" << endl;
            if (enrolledCourses.empty()) {
                cout << "  (none yet)" << endl;
            }

            for (int i = 0; i < (int)enrolledCourses.size(); i++) {
                cout << "  Course ID: " << enrolledCourses[i] << "  |  Grade: ";
                if (grades[i] < 0.0) 
                    cout << "N/A" << endl;
                else 
                    cout << fixed << setprecision(1) << grades[i] << endl;
            }
        } else if (choice == 2) {
            cout << fixed << setprecision(2);
            cout << "\n  Cumulative GPA: " << gpa << endl;
        } else if (choice == 3) {
            viewTimetable(admin);
        } else if (choice == 4) {
            viewTuitionStatus();
        } else if (choice == 5) {
            printTranscript();
        }

        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }
}
