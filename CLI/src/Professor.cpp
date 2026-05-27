#include "../include/professor.h"
#include "../include/admin.h"
#include "../include/IDgenerator.h"
#include "../include/LoginException.h"
#include <iostream>
#include <fstream>
using namespace std;

//clear window
static void clearScreen(){
    // ANSI escape clears the window of terminal
    cout << "\033[2J\033[H";
}

// ── string binary helpers
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
Professor::Professor(string name, string email, string password, string dept)
    : User(IDgenerator::generateProfessorID(), name, email, password),
      department(dept) {
    cout << "[Professor] '" << name << "' (ID: " << userId << ") created. Dept: " << dept << endl;
}

// Load-from-file constructor: ID already known
Professor::Professor(string id, string name, string email, string password, string dept) : User(id, name, email, password), department(dept) {}

Professor::~Professor() {
    cout << "[Professor] '" << name << "' (ID: " << userId << ") destroyed." << endl;
}

// ── getters
string Professor::getDepartment() const{ 
    return department;
}
vector<string> Professor::getCourseIds() const{
    return taughtCourseIds;
}

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

// 1. View Students enrolled in the professor's courses
void Professor::viewMyStudents(const Admin& admin) const {
    cout << "\n--- Roster of Students in Your Courses ---" << endl;
    if (taughtCourseIds.empty()) {
        cout << "You are not assigned to any courses." << endl;
        return;
    }
    
    // We reuse Admin's detail viewer safely to show student lists per course
    for (const string& courseId : taughtCourseIds) {
        admin.viewCourseDetail(courseId);
    }
}

// 2. View specific details for a course taught by this professor
void Professor::viewMyCourseDetail(const Admin& admin) const {
    if (taughtCourseIds.empty()) {
        cout << "You have no assigned courses to view." << endl;
        return;
    }

    string courseId;
    cout << "Enter Course ID to view: ";
    cin >> courseId;
    cin.ignore();

    // Verify ownership
    bool ownsCourse = false;
    for (const string& id : taughtCourseIds) {
        if (id == courseId) { ownsCourse = true; break; }
    }

    if (!ownsCourse) {
        cout << "[Error] You are not authorized to view Course '" << courseId << "'." << endl;
        return;
    }

    admin.viewCourseDetail(courseId);
}

// 3. Create an assignment
void Professor::createAssignment() {
    if (taughtCourseIds.empty()) {
        cout << "Cannot create assignments. You have no courses assigned." << endl;
        return;
    }

    string courseId, title;
    cout << "Enter Course ID for assignment: ";
    getline(cin, courseId);
    
    // Verify ownership
    bool ownsCourse = false;
    for (const string& id : taughtCourseIds) {
        if (id == courseId) { ownsCourse = true; break; }
    }
    if (!ownsCourse) {
        cout << "[Error] You do not teach course " << courseId << endl;
        return;
    }

    cout << "Enter Assignment Title/Description: ";
    getline(cin, title);

    assignments.push_back("[" + courseId + "] " + title);
    cout << "[Success] Assignment created: " << title << endl;
}

// 4. Make an announcement
void Professor::makeAnnouncement() {
    if (taughtCourseIds.empty()) {
        cout << "Cannot make announcements. You have no courses assigned." << endl;
        return;
    }

    string courseId, msg;
    cout << "Enter Course ID for announcement: ";
    getline(cin, courseId);

    // Verify ownership
    bool ownsCourse = false;
    for (const string& id : taughtCourseIds) {
        if (id == courseId) { ownsCourse = true; break; }
    }
    if (!ownsCourse) {
        cout << "[Error] You do not teach course " << courseId << endl;
        return;
    }

    cout << "Enter Announcement Message: ";
    getline(cin, msg);

    announcements.push_back("[" + courseId + "] " + msg);
    cout << "[Success] Announcement posted contextually." << endl;
}

// 4b. View Assignments
void Professor::viewAssignments() const {
    cout << "\n--- My Assignments ---" << endl;
    if (assignments.empty()) {
        cout << "  (none)" << endl;
        return;
    }
    for (size_t i = 0; i < assignments.size(); i++) {
        cout << "  " << i + 1 << ". " << assignments[i] << endl;
    }
}

// 4c. View Announcements
void Professor::viewAnnouncements() const {
    cout << "\n--- My Announcements ---" << endl;
    if (announcements.empty()) {
        cout << "  (none)" << endl;
        return;
    }
    for (size_t i = 0; i < announcements.size(); i++) {
        cout << "  " << i + 1 << ". " << announcements[i] << endl;
    }
}

// 5. Grade a student using Admin infrastructure
void Professor::gradeStudent(Admin& admin) {
    if (taughtCourseIds.empty()) {
        cout << "You cannot assign grades because you teach no courses." << endl;
        return;
    }

    string studentId, courseId;
    double score;

    cout << "Enter Course ID: "; getline(cin, courseId);
    
    // Verify professor is authorized to grade this specific class
    bool ownsCourse = false;
    for (const string& id : taughtCourseIds) {
        if (id == courseId) { ownsCourse = true; break; }
    }
    if (!ownsCourse) {
        cout << "[Error] You are not authorized to grade course " << courseId << endl;
        return;
    }

    cout << "Enter Student ID: "; getline(cin, studentId);
    cout << "Enter Score (0-100): "; cin >> score; cin.ignore();

    try {
        // Safely pass execution to Admin to validate score, update Student profile, and save to database
        admin.assignGrade(studentId, courseId, score);
    } catch (ValidationException& ve) {
        cout << "[Validation Error] " << ve.what() << endl;
    }
}

// UPDATED INTERACTIVE PANEL
void Professor::displayPanelWithAdmin(Admin& admin) {
    int choice = -1;
    while (choice != 0) {
        clearScreen();
        cout << "\n=== Professor Panel  |  " << name << "  |  ID: " << userId << "  |  Dept: " << department << " ===" << endl;
        cout << "  1. View my assigned courses" << endl;
        cout << "  2. View course details (and enrolled students)" << endl;
        cout << "  3. View roster of all my students" << endl;
        cout << "  4. Create course assignment" << endl;
        cout << "  5. Post an announcement" << endl;
        cout << "  6. Grade a student" << endl;
        cout << "  7. View my assignments" << endl;
        cout << "  8. View my announcements" << endl;
        cout << "  0. Logout" << endl;
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "\n--- Assigned Courses ---" << endl;
                if (taughtCourseIds.empty()) cout << "  (none assigned yet)" << endl;
                for (const string& id : taughtCourseIds) cout << "  - " << id << endl;
                break;
            case 2:
                viewMyCourseDetail(admin);
                break;
            case 3:
                viewMyStudents(admin);
                break;
            case 4:
                createAssignment();
                break;
            case 5:
                makeAnnouncement();
                break;
            case 6:
                gradeStudent(admin);
                break;
            case 7:
                viewAssignments();
                break;
            case 8:
                viewAnnouncements();
                break;
            case 0:
                cout << "Logging out of Professor panel..." << endl;
                break;
            default:
                cout << "Invalid choice. Try again." << endl;
        }

        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }
}

// binary file I/O
void Professor::writeToBinaryFull(ofstream& out) const {
    User::writeToBinary(out);   // userId, name, email, password

    writeStr(out, department);

    int count = (int)taughtCourseIds.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++) {
        writeStr(out, taughtCourseIds[i]);
    }

    // Save assignments
    int assignCount = (int)assignments.size();
    out.write(reinterpret_cast<const char*>(&assignCount), sizeof(assignCount));
    for (int i = 0; i < assignCount; i++) {
        writeStr(out, assignments[i]);
    }

    // Save announcements
    int annCount = (int)announcements.size();
    out.write(reinterpret_cast<const char*>(&annCount), sizeof(annCount));
    for (int i = 0; i < annCount; i++) {
        writeStr(out, announcements[i]);
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

    // Load assignments
    int assignCount = 0;
    in.read(reinterpret_cast<char*>(&assignCount), sizeof(assignCount));
    assignments.clear();
    for (int i = 0; i < assignCount; i++) {
        string a; readStr(in, a);
        assignments.push_back(a);
    }

    // Load announcements
    int annCount = 0;
    in.read(reinterpret_cast<char*>(&annCount), sizeof(annCount));
    announcements.clear();
    for (int i = 0; i < annCount; i++) {
        string an; readStr(in, an);
        announcements.push_back(an);
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

string Professor::getRole() const { 
    return "Professor";
}

void Professor::displayPanel() {
    int choice = 0;
    cout << "\n=== Professor Panel  |  " << name << "  |  ID: " << userId << "  |  Dept: " << department << " ===" << endl;
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
}
