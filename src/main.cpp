// ==== MENU-DRIVEN ====
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <stdexcept>
#include "../include/IDgenerator.h"
#include "../include/exceptions.h"
#include "../include/user.h"
#include "../include/student.h"
#include "../include/professor.h"
#include "../include/admin.h"
#include "../include/course.h"
#include "../include/assignment.h"
#include "../include/grade.h"
using namespace std;

static vector<Student*> gStudents;
static vector<Professor*> gProfessors;
static vector<Course*> gCourses;
static Admin* gAdmin = nullptr;

//clear window
static void clearScreen(){
    // ANSI escape clears the window of terminal
    cout << "\033[2J\033[H";
}

//wait user response
static void waitEnter() {
    cout << "\n  Press ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Divider line
static void divider(const string& title = "") {
    cout << "\n══════════════════════════════════════════════\n";
    if (!title.empty()) cout << "  " << title << "\n"
    << "══════════════════════════════════════════════\n";
}

// Read a non-blank LINE, re-prompting until the user types something
static string readLine(const string& prompt) {
    string val;
    while (true) {
        cout << prompt;
        if (getline(cin, val) && !val.empty()) return val;
        cout << "  [!] Cannot be empty — try again.\n";
    }
}

// Read an INTEGER in range of [lo, hi], loops until valid
static int readInt(const string& prompt, int lo, int hi) {
    int val;
    while (true) { //infinite loop
        cout << prompt;
        if (cin >> val && val >= lo && val <= hi) { //check the user input
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); //clear-up
            return val; //return valid input
        }
        cin.clear(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Enter a whole number between " << lo << " and " << hi << ".\n";
    }
}

// Read a DOUBLE in [lo, hi]
static double readDouble(const string& prompt, double lo, double hi) {
    double val;
    while (true) {
        cout << prompt;
        if (cin >> val && val >= lo && val <= hi) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Enter a number between " << lo << " and " << hi << ".\n";
    }
}

// (Yes / No) confirmation — returns true for 'y'
static bool confirm(const string& question) {
    cout << "  " << question << " [Y/n]: ";
    string ans;
    getline(cin, ans);
    return (ans == "y" || ans == "Y");
}

//Search helpers
static Student* findStudent(const string& id) {
    for (Student* s : gStudents){
        if (s && s->getUserId() == id){
            return s;
        }
    }
    return nullptr;
}
static Professor* findProfessor(const string& id) {
    for (Professor* p : gProfessors){
        if (p && p->getUserId() == id){
            return p;
        }
    }
    return nullptr;
}
static Course* findCourse (const string& id) {
    for (Course* c : gCourses){
        if (c && c->getCourseID() == id){
            return c;
        }
    }
    return nullptr;
}

// Print a numbered list of courses; user picks one (0 to cancel).
// Returns nullptr on cancel or empty list.
static Course* pickCourse(const string& heading) {
    if (gCourses.empty()){
        cout << "\n  [!] No courses in the system.\n";
        return nullptr;
    }

    divider(heading);
    for (size_t i = 0; i < gCourses.size(); ++i){
        if (i < 9){ 
            cout << "  [0" << i + 1 << "]  ";
        }else{
            cout << "  ["  << i + 1 << "]  ";
        }
        cout << gCourses[i]->getCourseID() << " : " << gCourses[i]->getCourseName() << "\n";
    }
    cout << "  --------------------------------------------------\n";
    cout << "  [00]  Go Back / Cancel\n";
    int ch = readInt("\n Enter Choice (eg. 01, 10 or 00 to cancel): ", 0, static_cast<int>(gCourses.size()));
    
    if (ch == 0) {
        return nullptr;  //"00" return nothing
    } else {
        return gCourses[ch - 1];  // Return the actual course they picked
    }
}

// Pick one of the enrolled courses of a student
static Course* pickEnrolledCourse(Student* s, const string& heading) {
    auto vec = s->getEnrolledCourses();
    if (vec.empty()) { cout << "\n  [!] Not enrolled in any course.\n"; return nullptr; }
    divider(heading);
    for (size_t i = 0; i < vec.size(); ++i)
        cout << "  [" << i + 1 << "]  "
             << vec[i]->getCourseID() << "  —  "
             << vec[i]->getCourseName() << "\n";
    cout << "  [0]  Cancel\n";
    int ch = readInt("  Choice: ", 0, static_cast<int>(vec.size()));

    if(ch == 0){
        return nullptr;
    }else{
        return vec[ch - 1];
    }
}

// Pick a student from an enrolled list inside a course
static Student* pickStudentFromCourse(Course* c) {
    auto vec = c->getEnrolledStudents();
    if (vec.empty()) { cout << "\n  [!] No students enrolled.\n"; return nullptr; }
    divider("Select student");
    for (size_t i = 0; i < vec.size(); ++i)
        cout << "  [" << i + 1 << "]  "
             << vec[i]->getUserId() << "  " << vec[i]->getName() << "\n";
    cout << "  [0]  Cancel\n";
    int ch = readInt("  Choice: ", 0, static_cast<int>(vec.size()));

    if(ch == 0){
        return nullptr;
    } else {
        return vec[ch - 1];
    }
}


// BETA SYSTEM
static void seedData() {
    // Admin
    gAdmin = new Admin("Dr. Karimov", "karimov@iut.uz", "admin123", "Registry");

    // Professors
    Professor* p1 = gAdmin->createProfessor("Prof. Aliyev",  "aliyev@iut.uz",  "prof123", "Computer Science");
    Professor* p2 = gAdmin->createProfessor("Prof. Rahimov", "rahimov@iut.uz", "prof456", "Mathematics");
    gProfessors.push_back(p1);
    gProfessors.push_back(p2);

    // Students
    Student* s1 = gAdmin->createStudent("Akmal Toshmatov",  "akmal@iut.uz",   "pass123");
    Student* s2 = gAdmin->createStudent("Marin Nazarova", "marin@iut.uz",  "pass456");
    Student* s3 = gAdmin->createStudent("Bobur Yusupov",    "bobur@iut.uz",    "pass789");
    gStudents.push_back(s1);
    gStudents.push_back(s2);
    gStudents.push_back(s3);

    // Courses
    Course* cpp  = new Course("CS101",   "C++ Programming", 3, 30);
    Course* calc = new Course("MATH201", "Calculus II",     4, 30);
    Course* phys = new Course("PHY102",  "Physics 2",       3, 30);
    gCourses.push_back(cpp);
    gCourses.push_back(calc);
    gCourses.push_back(phys);

    // Assign professors
    gAdmin->assignProfessorToCourse(*p1, *cpp);
    gAdmin->assignProfessorToCourse(*p2, *calc);
    gAdmin->assignProfessorToCourse(*p1, *phys);

    // Enroll students
    s1->enrollCourse(*cpp);
    s1->enrollCourse(*calc);
    s2->enrollCourse(*cpp);
    s2->enrollCourse(*calc);
    s3->enrollCourse(*calc);
    s3->enrollCourse(*phys);

    // Assignments
    p1->createAssignment(*cpp,  "Lab 1",    "Hello World in C++",         "15-05-2025", 100.0);
    p1->createAssignment(*cpp,  "Lab 2",    "Implement a class hierarchy", "30-05-2025", 100.0);
    p2->createAssignment(*calc, "HW 1",     "Convergence tests",           "20-05-2025",  50.0);
    p1->createAssignment(*phys, "Report 1", "Electric field lab report",   "25-05-2025",  80.0);

    // Grades
    p1->assignGrade(*s1, *cpp,  92.0);
    p1->assignGrade(*s2, *cpp,  78.5);
    p2->assignGrade(*s1, *calc, 85.0);
    p2->assignGrade(*s2, *calc, 60.0);
    p2->assignGrade(*s3, *calc, 45.0);

    // Materials & announcements 
    cpp->addMaterial("https://cppreference.com");
    cpp->addAnnouncement("Midterm exam on 10-06-2025. Bring student ID.");
    calc->addMaterial("Stewart Calculus 8th edition PDF");

    cout << "\n  [System] Seed data ready.\n";
}

//  LOGIN SCREEN ============================================================
//  Tries Admin → Professors → Students in order.
//  Returns the authenticated User* or nullptr on failure.
static User* doLogin() {
    clearScreen();
    divider("Login");
    string email = readLine("  Email    : ");
    string pass  = readLine("  Password : ");

    // Admin
    if (gAdmin) {
        try { if (gAdmin->login(email, pass)) return gAdmin; }
        catch (...) {}
    }
    // Professors
    for (Professor* p : gProfessors) {
        try { if (p->login(email, pass)) return p; }
        catch (...) {}
    }
    // Students
    for (Student* s : gStudents) {
        try { if (s->login(email, pass)) return s; }
        catch (...) {}
    }

    cout << "\n  [!] Invalid credentials.\n";
    waitEnter();
    return nullptr;
}

//  STUDENT MENU ============================================================
static void portalStudent(Student* s) {
    int ch;
    do {
        clearScreen();
        divider("STUDENT PORTAL  ─  " + s->getName() + "  [" + s->getUserId() + "]");
        cout << "  [1]  My info\n"
             << "  [2]  My enrolled courses\n"
             << "  [3]  Enroll in a course\n"
             << "  [4]  Drop a course\n"
             << "  [5]  My grades\n"
             << "  [6]  My transcript\n"
             << "  [7]  Submit an assignment\n"
             << "  [8]  View course materials\n"
             << "  [9]  View course announcements\n"
             << "  [10] Save my grades to file\n"
             << "  [11] Update profile\n"
             << "  [12] Change password\n"
             << "  [0]  Logout\n";
        ch = readInt("  > ", 0, 12);

        //1. My info
        if (ch == 1) {
            clearScreen();
            s->displayInfo();
            waitEnter();
        }

        //2. Enrolled courses
        else if (ch == 2) {
            clearScreen();
            divider("Enrolled Courses");
            auto vec = s->getEnrolledCourses();
            if (vec.empty()) cout << "  (not enrolled in any course)\n";
            else for (Course* c : vec) {
                cout << *c;
                // show assignments in that course
                auto asgns = c->getAssignments();
                if (!asgns.empty()) {
                    cout << "  Assignments:\n";
                    for (const Assignment& a : asgns)
                        cout << "    • " << a.getTitle()
                             << "  (due: " << a.getDueDate()
                             << ", max: " << a.getMaxScore() << ")\n";
                }
                cout << "\n";
            }
            waitEnter();
        }

        //3. Enroll in a course
        else if (ch == 3) {
            clearScreen();
            Course* c = pickCourse("Available Courses");
            if (!c) { 
                waitEnter();
                continue;
            }
            try {
                s->enrollCourse(*c);
                cout << "\n  [OK] Enrolled in " << c->getCourseName() << ".\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        //4. Drop a course
        else if (ch == 4) {
            clearScreen();
            Course* c = pickEnrolledCourse(s, "Drop a Course");
            if (!c) {
                waitEnter();
                continue;
            }
            if (!confirm("Drop \"" + c->getCourseName() + "\"?")) continue;
            try {
                s->dropCourse(*c);
                cout << "\n  [OK] Dropped " << c->getCourseName() << ".\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // 5. My grades
        else if (ch == 5) {
            clearScreen();
            s->viewGrades();
            waitEnter();
        }

        // 6. Transcript
        else if (ch == 6) {
            clearScreen();
            cout << s->viewTranscript();
            waitEnter();
        }

        // 7. Submit assignment
        else if (ch == 7) {
            clearScreen();
            Course* c = pickEnrolledCourse(s, "Select course");
            if (!c) { 
                waitEnter();
                continue;
            }

            auto asgns = c->getAssignments();
            if (asgns.empty()) {
                cout << "\n  [!] No assignments in this course.\n";
                waitEnter(); continue;
            }
            divider("Select Assignment");
            for (size_t i = 0; i < asgns.size(); ++i)
                cout << "  [" << i + 1 << "]  " << asgns[i].getTitle()
                     << "  (due: " << asgns[i].getDueDate() << ")\n";
            cout << "  [0]  Cancel\n";
            int pick = readInt("  Choice: ", 0, static_cast<int>(asgns.size()));
            if (pick == 0) continue;
            try {
                s->submitAssignment(asgns[pick - 1]);
                cout << "\n  [OK] Assignment submitted.\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // 8. Course materials
        else if (ch == 8) {
            clearScreen();
            Course* c = pickEnrolledCourse(s, "Select course");
            if (!c) { 
                waitEnter();
                continue;
            }
            divider("Materials  —  " + c->getCourseName());
            auto mats = c->getMaterials();
            if (mats.empty()) cout << "  (no materials posted yet)\n";
            else for (size_t i = 0; i < mats.size(); ++i)
                cout << "  " << i + 1 << ". " << mats[i] << "\n";
            waitEnter();
        }

        // 9. Course announcements
        else if (ch == 9) {
            clearScreen();
            Course* c = pickEnrolledCourse(s, "Select course");
            if (!c) {
                waitEnter();
                continue;
            }
            divider("Announcements  —  " + c->getCourseName());
            auto anns = c->getAnnouncements();
            if (anns.empty()) cout << "  (no announcements)\n";
            else for (size_t i = 0; i < anns.size(); ++i)
                cout << "  " << i + 1 << ". " << anns[i] << "\n";
            waitEnter();
        }

        //10. Save grades to file 
        else if (ch == 10) {
            clearScreen();
            auto grades = s->getGrades();

            if (grades.empty()) {
                cout << "\n  [!] No grades to save.\n";
                waitEnter(); 
                continue;
            }

            string fname = readLine("  Filename (e.g. akmal_grades.txt): ");

            try {
                for (const Grade& g : grades)
                    g.saveToFile(fname);
                cout << "\n  [OK] " << grades.size()
                     << " grade(s) saved to \"" << fname << "\".\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        //11. Update profile
        else if (ch == 11) {
            clearScreen();
            divider("Update Profile");
            string newName  = readLine("  New name  (current: " + s->getName()  + "): ");
            string newEmail = readLine("  New email (current: " + s->getEmail() + "): ");
            try {
                s->updateProfile(newName, newEmail);
                cout << "\n  [OK] Profile updated.\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        //12. Change password
        else if (ch == 12) {
            clearScreen();
            divider("Change Password");
            string oldPass = readLine("  Current password : ");
            string newPass = readLine("  New password     : ");
            string confirm2 = readLine("  Confirm new pass : ");
            if (newPass != confirm2) {
                cout << "\n  [!] Passwords do not match.\n";
            } else {
                try {
                    s->changePassword(oldPass, newPass);
                } catch (const exception& e) {
                    cout << "\n  [ERROR] " << e.what() << "\n";
                }
            }
            waitEnter();
        }

    } while (ch != 0);

    s->logout();
}

//  PROFESSOR MENU ============================================================
static void portalProfessor(Professor* p) {
    int ch;
    do {
        clearScreen();
        divider("PROFESSOR PORTAL  ─  " + p->getName() + "  [" + p->getUserId() + "]");
        cout << "  [1]  My info\n"
             << "  [2]  My courses\n"
             << "  [3]  View course roster\n"
             << "  [4]  Assign grade to student\n"
             << "  [5]  Update an existing grade\n"
             << "  [6]  Set feedback on a grade\n"
             << "  [7]  Create assignment\n"
             << "  [8]  Post material\n"
             << "  [9]  Post announcement\n"
             << "  [10] Generate course report\n"
             << "  [11] Save course to file\n"
             << "  [12] Load course from file\n"
             << "  [13] Change password\n"
             << "  [0]  Logout\n";
        ch = readInt("  > ", 0, 13);

        // ── 1. Info ───────────────────────────────────────────────────────────
        if (ch == 1) {
            clearScreen();
            p->displayInfo();
            waitEnter();
        }

        // ── 2. My courses ─────────────────────────────────────────────────────
        else if (ch == 2) {
            clearScreen();
            divider("My Courses");
            auto tc = p->getTaughtCourses();
            if (tc.empty()) cout << "  (no courses assigned yet)\n";
            else for (Course* c : tc) {
                cout << *c;
                cout << "  Enrolled : " << c->getEnrolledCount()
                     << " / " << c->getCapacity() << "\n\n";
            }
            waitEnter();
        }

        // ── 3. Roster ─────────────────────────────────────────────────────────
        else if (ch == 3) {
            clearScreen();
            Course* c = pickCourse("Select Course — Roster");
            if (!c) {
                waitEnter();
                continue;
            }
            p->manageRoster(*c);
            waitEnter();
        }

        // ── 4. Assign grade ───────────────────────────────────────────────────
        else if (ch == 4) {
            clearScreen();
            Course* c = pickCourse("Select Course — Assign Grade");
            if (!c) { 
                waitEnter();
                continue;
            }
            Student* st = pickStudentFromCourse(c);
            if (!st) { 
                waitEnter();
                continue;
            }

            double score = readDouble(
                "  Score for " + st->getName() + " (0–100): ", 0.0, 100.0);
            try {
                p->assignGrade(*st, *c, score);
                cout << "\n  [OK] Grade assigned.\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // ── 5. Update existing grade ──────────────────────────────────────────
        else if (ch == 5) {
            clearScreen();
            Course* c = pickCourse("Select Course — Update Grade");
            if (!c) { 
                waitEnter();
                continue;
            }
            Student* st = pickStudentFromCourse(c);
            if (!st) { 
                waitEnter();
                continue;
            }

            // Find the grade object in the student's list for this course
            auto grades = st->getGrades();
            bool found  = false;
            for (Grade& g : grades) {
                if (g.getCourse() && g.getCourse()->getCourseID() == c->getCourseID()) {
                    cout << "\n  Current score: " << g.getScore()
                         << "  (" << g.getLetterGrade() << ")\n";
                    double newScore = readDouble("  New score (0–100): ", 0.0, 100.0);
                    try {
                        g.setScore(newScore);
                        st->calculateGPA();
                        cout << "\n  [OK] Grade updated.\n";
                    } catch (const exception& e) {
                        cout << "\n  [ERROR] " << e.what() << "\n";
                    }
                    found = true;
                    break;
                }
            }
            if (!found)
                cout << "\n  [!] No existing grade — use option 4 to assign one first.\n";
            waitEnter();
        }

        // ── 6. Set feedback ───────────────────────────────────────────────────
        else if (ch == 6) {
            clearScreen();
            Course* c = pickCourse("Select Course — Feedback");
            if (!c) { 
                waitEnter();
                continue;
            }
            Student* st = pickStudentFromCourse(c);
            if (!st) { 
                waitEnter();
                continue;
         }

            auto grades = st->getGrades();
            bool found  = false;
            for (Grade& g : grades) {
                if (g.getCourse() && g.getCourse()->getCourseID() == c->getCourseID()) {
                    string fb = readLine("  Feedback: ");
                    try {
                        g.setFeedback(fb);
                        cout << "\n  [OK] Feedback saved.\n";
                    } catch (const exception& e) {
                        cout << "\n  [ERROR] " << e.what() << "\n";
                    }
                    found = true;
                    break;
                }
            }
            if (!found)
                cout << "\n  [!] No grade on record for this student in this course.\n";
            waitEnter();
        }

        // ── 7. Create assignment ──────────────────────────────────────────────
        else if (ch == 7) {
            clearScreen();

            Course* c = pickCourse("Select Course — New Assignment");

            if (!c) { 
                waitEnter();
                continue;
            }
            divider("Create Assignment");

            string title = readLine("  Title               : ");
            string desc  = readLine("  Description         : ");
            string due   = readLine("  Due date (DD-MM-YYYY): ");
            double maxS  = readDouble("  Max score            : ", 1.0, 1000.0);


            try {
                p->createAssignment(*c, title, desc, due, maxS);
                cout << "\n  [OK] Assignment created.\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // ── 8. Post material ──────────────────────────────────────────────────
        else if (ch == 8) {
            clearScreen();

            Course* c = pickCourse("Select Course — Post Material");
            if(!c){
                waitEnter(); 
                continue;
            }

            string mat = readLine("  Material (URL or description): ");

            try {
                p->postMaterial(mat, *c);
                cout << "\n  [OK] Material posted.\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }

            waitEnter();
        }

        // ── 9. Post announcement ──────────────────────────────────────────────
        else if (ch == 9) {
            clearScreen();

            Course* c = pickCourse("Select Course — Announcement");

            if (!c) { 
                waitEnter();
                continue;
            }
            
            string msg = readLine("  Announcement: ");

            try {
                c->addAnnouncement(msg);
                cout << "\n  [OK] Announcement posted.\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // ── 10. Generate report ───────────────────────────────────────────────
        else if (ch == 10) {
            clearScreen();
            Course* c = pickCourse("Select Course — Report");
            if (!c) { 
                waitEnter();
                continue;
            }

            divider("Course Report");

            cout << p->generateReport(*c);
            waitEnter();
        }

        // ── 11. Save course to file ───────────────────────────────────────────
        else if (ch == 11) {
            clearScreen();

            Course* c = pickCourse("Select Course — Save to File");
            if (!c) { 
                waitEnter();
                continue;
            }
            string fname = readLine("  Filename (e.g. cs101.txt): ");

            try {
                c->saveToFile(fname);
                cout << "\n  [OK] Saved to \"" << fname << "\".\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }

            waitEnter();
        }

        // ── 12. Load course from file ─────────────────────────────────────────
        else if (ch == 12) {
            clearScreen();

            string fname = readLine("  Filename to load: ");

            try {
                Course* loaded = new Course(Course::loadFromFile(fname));
                gCourses.push_back(loaded);
                cout << "\n  [OK] Loaded course: "
                     << loaded->getCourseName() << " (" << loaded->getCourseID() << ")\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // ── 13. Change password ───────────────────────────────────────────────
        else if (ch == 13) {
            clearScreen();

            divider("Change Password");
            string oldP = readLine("  Current password : ");
            string newP = readLine("  New password     : ");
            string newP2 = readLine("  Confirm new pass : ");

            if (newP != newP2) {
                cout << "\n  [!] Passwords do not match.\n";
            } else {
                try {
                    p->changePassword(oldP, newP);
                } catch (const exception& e) {
                    cout << "\n  [ERROR] " << e.what() << "\n";
                }
            }
            waitEnter();
        }

    } while (ch != 0);

    p->logout();
}

//  ADMIN PORTAL ============================================================
static void portalAdmin(Admin* a) {
    int ch;
    do {
        clearScreen();
        divider("ADMIN PORTAL  ─  " + a->getName() + "  [" + a->getUserId() + "]");
        cout << "  ── USER MANAGEMENT ──────────────────\n"
             << "  [1]  Create student account\n"
             << "  [2]  Create professor account\n"
             << "  [3]  List all students\n"
             << "  [4]  List all professors\n"
             << "  [5]  Find student by ID\n"
             << "  [6]  Find professor by ID\n"
             << "  [7]  Delete a user\n"
             << "  ── COURSE MANAGEMENT ────────────────\n"
             << "  [8]  Create a course\n"
             << "  [9]  List all courses (detailed)\n"
             << "  [10] Assign professor to course\n"
             << "  [11] Remove a course\n"
             << "  [12] Save course to file\n"
             << "  [13] Load course from file\n"
             << "  ── PERMISSIONS & REPORTS ────────────\n"
             << "  [14] Manage my permissions\n"
             << "  [15] System report\n"
             << "  [16] My info\n"
             << "  [0]  Logout\n";
        ch = readInt("  > ", 0, 16);

        // ── 1. Create student 
        if (ch == 1) {
            clearScreen();
            divider("Create Student Account");
            string name  = readLine("  Full name  : ");
            string email = readLine("  Email      : ");
            string pass  = readLine("  Password   : ");
            try {
                Student* s = a->createStudent(name, email, pass);
                gStudents.push_back(s);
                cout << "\n  [OK] Student created. ID = " << s->getUserId() << "\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // ── 2. Create professor 
        else if (ch == 2) {
            clearScreen();

            divider("Create Professor Account");
            string name  = readLine("  Full name  : ");
            string email = readLine("  Email      : ");
            string pass  = readLine("  Password   : ");
            string dept  = readLine("  Department : ");

            try {
                Professor* p = a->createProfessor(name, email, pass, dept);
                gProfessors.push_back(p);
                cout << "\n  [OK] Professor created. ID = " << p->getUserId() << "\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // ── 3. List students 
        else if (ch == 3) {
            clearScreen();
            a->viewAllStudents(gStudents);
            waitEnter();
        }

        // ── 4. List professors 
        else if (ch == 4) {
            clearScreen();
            a->viewAllProfessors(gProfessors);
            waitEnter();
        }

        // ── 5. Find student by ID 
        else if (ch == 5) {
            clearScreen();

            string id = readLine("  Student ID: ");
            try {
                Student* s = a->findStudentByID(gStudents, id);
                s->displayInfo();
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // ── 6. Find professor by ID 
        else if (ch == 6) {
            clearScreen();
            string id = readLine("  Professor ID: ");
            try {
                Professor* p = a->findProfessorByID(gProfessors, id);
                p->displayInfo();
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // ── 7. Delete user 
        else if (ch == 7) {
            clearScreen();
            divider("Delete User");
            cout << "  Delete  [1] Student   [2] Professor   [0] Cancel\n";
            int type = readInt("  > ", 0, 2);
            if (type == 0) continue;

            if (type == 1) {
                a->viewAllStudents(gStudents);

                if (gStudents.empty()){
                    waitEnter();
                    continue;
                }

                string id = readLine("  Student ID to delete: ");

                bool found = false;

                for (auto it = gStudents.begin(); it != gStudents.end(); ++it) {
                    if ((*it)->getUserId() == id) {
                        if (!confirm("Delete student " + (*it)->getName() + "?"))
                            break;

                        User* ptr = *it;
                        gStudents.erase(it);

                        try {
                            a->deleteUser(ptr);
                            cout << "\n  [OK] Student deleted.\n";
                        } catch (const exception& e) {
                            cout << "\n  [ERROR] " << e.what() << "\n";
                        }
                        found = true; 
                        break;
                    }
                }
                if (!found) cout << "\n  [!] ID not found.\n";
            } else {
                a->viewAllProfessors(gProfessors);
                if (gProfessors.empty()){
                    waitEnter(); 
                    continue;
                }
                string id = readLine("  Professor ID to delete: ");
                bool found = false;
                for (auto it = gProfessors.begin(); it != gProfessors.end(); ++it) {
                    if ((*it)->getUserId() == id) {
                        if (!confirm("Delete professor " + (*it)->getName() + "?"))
                            break;

                        User* ptr = *it;
                        gProfessors.erase(it);

                        try {
                            a->deleteUser(ptr);
                            cout << "\n  [OK] Professor deleted." << endl;;
                        } catch (const exception& e) {
                            cout << "\n  [ERROR] " << e.what() << endl;
                        }
                        found = true; 
                        break;
                    }
                }
                if (!found) cout << "\n  [!] ID not found." << endl;
            }
            waitEnter();
        }

        // ── 8. Create course 
        else if (ch == 8) {
            clearScreen();
            divider("Create Course");
            string cid = readLine("  Course ID   : ");
            if (findCourse(cid)) {
                cout << "\n  [!] Course ID already exists." << endl;
                waitEnter();
                continue;
            }
            string cname = readLine("  Course name : ");
            int cred = readInt("  Credits     : ", 1, 20);
            int cap = readInt("  Capacity    : ", 1, 500);
            try {
                Course* c = new Course(cid, cname, cred, cap);
                gCourses.push_back(c);
                cout << "\n  [OK] Course created.\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << endl;
            }
            waitEnter();
        }

        // ── 9. List all courses 
        else if (ch == 9) {
            clearScreen();

            divider("All Courses  (" + to_string(gCourses.size()) + ")");

            if (gCourses.empty()) {
                cout << "  (none)\n"; 
                waitEnter();
                continue;
            }

            for (Course* c : gCourses) {
                cout << *c;
                cout << "  Instructor : ";
                if (c->getProfessor()){
                    cout << c->getProfessor()->getName();
                }else{                   
                    cout << "(none)" << endl;
                }

                auto mats = c->getMaterials();
                if (!mats.empty()) {
                    cout << "  Materials  :\n";
                    for (const string& m : mats) cout << "    • " << m << endl;
                }

                auto anns = c->getAnnouncements();
                if (!anns.empty()) {
                    cout << "  Announcements:\n";
                    for (const string& ann : anns) cout << "    ! " << ann << endl;
                }

                auto asgns = c->getAssignments();
                if (!asgns.empty()) {
                    cout << "  Assignments:\n";
                    for (const Assignment& asg : asgns)
                        cout << "    [" << asg.getAssignmentID() << "] " << asg.getTitle() << "  due " << asg.getDueDate() << "  max " << asg.getMaxScore() << endl;
                }
                cout << endl;
            }
            waitEnter();
        }

        // ── 10. Assign professor to course 
        else if (ch == 10) {
            clearScreen();
            a->viewAllProfessors(gProfessors);

            if (gProfessors.empty()) {
                waitEnter();
                continue;
            }

            string pid = readLine("  Professor ID: ");

            Professor* prof = findProfessor(pid);
            if (!prof) {
                cout << "\n  [!] Professor not found." << endl;
                waitEnter();
                continue;
            }
            Course* c = pickCourse("Select Course to Assign");

            if (!c) { 
                waitEnter();
                continue;
            }

            try {
                a->assignProfessorToCourse(*prof, *c);
                cout << "\n  [OK] " << prof->getName()
                     << " assigned to " << c->getCourseName() << ".\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << endl;
            }
            waitEnter();
        }

        // ── 11. Remove course 
        else if (ch == 11) {
            clearScreen();
            Course* c = pickCourse("Select Course — Remove");

            if (!c){
                waitEnter();
                continue;
            }
            if (!confirm("Permanently remove \"" + c->getCourseName() + "\"?")) 
                continue;

            // Erase from global list before admin deletes the heap object

            for (auto it = gCourses.begin(); it != gCourses.end(); ++it) {
                if (*it == c){
                    gCourses.erase(it);
                    break;
                }
            }
            try {
                a->removeCourse(c);
                cout << "\n  [OK] Course removed.\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // ── 12. Save course to file 
        else if (ch == 12) {
            clearScreen();
            Course* c = pickCourse("Select Course — Save to File");

            if (!c) {
                waitEnter();
                continue;
            }
            string fname = readLine("  Filename (e.g. cs101.txt): ");
            try {
                c->saveToFile(fname);
                cout << "\n  [OK] Saved to \"" << fname << "\".\n";
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << "\n";
            }
            waitEnter();
        }

        // ── 13. Load course from file 
        else if (ch == 13) {
            clearScreen();
            string fname = readLine("  Filename to load: ");
            try {
                Course* loaded = new Course(Course::loadFromFile(fname));
                gCourses.push_back(loaded);
                cout << "\n  [OK] Course loaded: " << loaded->getCourseName() << " (" << loaded->getCourseID() << ")" << endl;
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << endl;
            }
            waitEnter();
        }

        // ── 14. Manage permissions 
        else if (ch == 14) {
            clearScreen();
            
            divider("My Permissions");
            
            auto perms = a->getPermissions();
            
            cout << "  Current permissions:" << endl;
            
            for (const string& perm : perms) {
                cout << "    • " << perm << endl;
            }
            cout << "\n  [1] Add permission   [2] Remove permission   [0] Back\n";

            int opt = readInt("  > ", 0, 2);
            
            if (opt == 0) {
                continue;
            }
            
            string perm = readLine("  Permission string: ");
            
            try {
                if (opt == 1){
                    a->addPermission(perm);
                }else{
                    a->deletePermission(perm);
                }

                cout << "\n  [OK]  " << endl;
            } catch (const exception& e) {
                cout << "\n  [ERROR] " << e.what() << endl;
            }
            waitEnter();
        }

        // ── 15. System report 
        else if (ch == 15) {
            clearScreen();
            cout << a->generateReport(gStudents, gProfessors, gCourses);
            cout << "\nTotal users registered (live): " << User::getTotalUsers() << "\n";
            cout << "Total assignments created: " << Assignment::getTotalAssignments() << "\n";
            waitEnter();
        }

        // ── 16. My info 
        else if (ch == 16) {
            clearScreen();
            a->displayInfo();
            waitEnter();
        }

    } while (ch != 0);

    a->logout();
}

//MAIN 
int main() {
    // create DEMO datas
    seedData();

    int ch;
    do {
        clearScreen();
        divider("GRADE MANAGEMENT SYSTEM  ─  INHA University in Tashkent");

        cout << "  [1]  Login" << endl;
        cout << "  [2]  Browse courses (guest)" << endl;
        cout << "  [0]  Exit" << endl;
        ch = readInt("  > ", 0, 2);

        // ── Login 
        if (ch == 1) {
            User* user = doLogin();
            if (!user) 
                continue;

            string role = user->getRole();

            if (role == "Admin") {
            portalAdmin(dynamic_cast<Admin*>(user));
            } else if(role == "Professor"){
                portalProfessor(dynamic_cast<Professor*>(user));
            } else if(role == "Student"){
                portalStudent(dynamic_cast<Student*>(user));
            } else{
                cout << "\n [!] UKNOWN ROLE: " << role << endl;
                waitEnter();
            }
        }

        // GUEST
        else if (ch == 2) {
            clearScreen();
            
            divider("Available Courses  (" + to_string(gCourses.size()) + ")");

            if (gCourses.empty()){
                cout << "  (no courses available)\n";
            } else 
            for (Course* c : gCourses) {
                cout << "  " << c->getCourseID() << "  " << c->getCourseName() << "  [" << c->getEnrolledCount() << "/" << c->getCapacity() << " seats]";
                if (c->getProfessor()){
                    cout << "  —  " << c->getProfessor()->getName();
                }
                cout << "\n";
            }
            waitEnter();
        }

    } while (ch != 0);

    // Memory Cleanup
    clearScreen();
    cout << "  Shutting down...\n";
    for (Student* s : gStudents) delete s;
    for (Professor* p : gProfessors) delete p;
    for (Course* c : gCourses) delete c;
    delete gAdmin;

    cout << "  Goodbye.\n";
    return 0;
}