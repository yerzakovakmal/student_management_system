#include "../include/Admin.h"
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

// string helpers 
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

// constructor / destructor 
Admin::Admin(string name, string email, string password)
    : User(IDgenerator::generateAdminID(), name, email, password) {
    cout << "[Admin] '" << name << "' (ID: " << userId << ") created." << endl;
}

Admin::~Admin() {
    for (int i = 0; i < (int)students.size(); i++)   delete students[i];
    for (int i = 0; i < (int)professors.size(); i++) delete professors[i];
    cout << "[Admin] Session ended." << endl;
}

//  BINARY PERSISTENCE  (private helpers)

void Admin::saveStudents() const {
    ofstream out("students.dat", ios::binary | ios::trunc);
    if (!out.is_open())
        throw FileException("Cannot open students.dat for writing.");

    int count = (int)students.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++)
        students[i]->writeToBinaryFull(out);

    out.close();
    cout << "[Admin] " << count << " student(s) saved to students.dat" << endl;
}

void Admin::saveProfessors() const {
    ofstream out("professors.dat", ios::binary | ios::trunc);
    if (!out.is_open())
        throw FileException("Cannot open professors.dat for writing.");

    int count = (int)professors.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++)
        professors[i]->writeToBinaryFull(out);

    out.close();
    cout << "[Admin] " << count << " professor(s) saved to professors.dat" << endl;
}

void Admin::saveCourses() const {
    ofstream out("courses.dat", ios::binary | ios::trunc);
    if (!out.is_open())
        throw FileException("Cannot open courses.dat for writing.");

    int count = (int)courses.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++)
        writeCourseBinary(out, courses[i]);

    out.close();
    cout << "[Admin] " << count << " course(s) saved to courses.dat" << endl;
}

void Admin::loadStudents() {
    ifstream in("students.dat", ios::binary);
    if (!in.is_open()) {
        cout << "[Admin] students.dat not found — starting fresh." << endl;
        return;
    }

    int count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++) {
        // Temporary placeholders — readFromBinaryFull overwrites them all
        Student* s = new Student("__load__", "__load__@x.com", "placeholder");
        s->readFromBinaryFull(in);
        students.push_back(s);
    }
    in.close();
    cout << "[Admin] Loaded " << count << " student(s) from students.dat" << endl;
}

void Admin::loadProfessors() {
    ifstream in("professors.dat", ios::binary);
    if (!in.is_open()) {
        cout << "[Admin] professors.dat not found — starting fresh." << endl;
        return;
    }

    int count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++) {
        Professor* p = new Professor("__load__", "__load__@x.com",
                                     "placeholder", "UNKNOWN");
        p->readFromBinaryFull(in);
        professors.push_back(p);
    }
    in.close();
    cout << "[Admin] Loaded " << count << " professor(s) from professors.dat" << endl;
}

void Admin::loadCourses() {
    ifstream in("courses.dat", ios::binary);
    if (!in.is_open()) {
        cout << "[Admin] courses.dat not found — starting fresh." << endl;
        return;
    }

    int count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++) {
        Course c;
        readCourseBinary(in, c);
        courses.push_back(c);
    }
    in.close();
    cout << "[Admin] Loaded " << count << " course(s) from courses.dat" << endl;
}

void Admin::loadAllData() {
    loadStudents();
    loadProfessors();
    loadCourses();
}

// ════════════════════════════════════════════════════════════════════════════
//  USER MANAGEMENT
// ════════════════════════════════════════════════════════════════════════════

void Admin::createStudent(string name, string email, string password) {
    // Validation: password length
    if (password.size() < 6)
        throw ValidationException("Password must be at least 6 characters.");

    Student* s = new Student(name, email, password);
    students.push_back(s);
    saveStudents();   // persist immediately
    cout << "[Admin] Student created: " << name
         << " (ID: " << s->getUserId() << ")" << endl;
}

void Admin::createProfessor(string name, string email,
                            string password, string dept) {
    if (password.size() < 6)
        throw ValidationException("Password must be at least 6 characters.");
    if (dept.empty())
        throw ValidationException("Department cannot be empty.");

    Professor* p = new Professor(name, email, password, dept);
    professors.push_back(p);
    saveProfessors();
    cout << "[Admin] Professor created: " << name
         << " (ID: " << p->getUserId() << ")" << endl;
}

void Admin::removeStudent(string userId) {
    for (int i = 0; i < (int)students.size(); i++) {
        if (students[i]->getUserId() == userId) {
            cout << "[Admin] Removing student: " << students[i]->getName() << endl;
            delete students[i];
            students.erase(students.begin() + i);
            saveStudents();
            return;
        }
    }
    cout << "[Admin] Student ID '" << userId << "' not found." << endl;
}

void Admin::removeProfessor(string userId) {
    for (int i = 0; i < (int)professors.size(); i++) {
        if (professors[i]->getUserId() == userId) {
            cout << "[Admin] Removing professor: " << professors[i]->getName() << endl;
            delete professors[i];
            professors.erase(professors.begin() + i);
            saveProfessors();
            return;
        }
    }
    cout << "[Admin] Professor ID '" << userId << "' not found." << endl;
}

// ── function overloading: find by ID ──────────────────────────────────────
Student* Admin::findStudent(string userId) const {
    for (int i = 0; i < (int)students.size(); i++)
        if (students[i]->getUserId() == userId) return students[i];
    return NULL;
}

// find by name (byName flag distinguishes the overload signature)
Student* Admin::findStudent(string name, bool byName) const {
    for (int i = 0; i < (int)students.size(); i++)
        if (students[i]->getName() == name) return students[i];
    return NULL;
}

Professor* Admin::findProfessor(string userId) const {
    for (int i = 0; i < (int)professors.size(); i++)
        if (professors[i]->getUserId() == userId) return professors[i];
    return NULL;
}

Professor* Admin::findProfessor(string name, bool byName) const {
    for (int i = 0; i < (int)professors.size(); i++)
        if (professors[i]->getName() == name) return professors[i];
    return NULL;
}

// ════════════════════════════════════════════════════════════════════════════
//  COURSE MANAGEMENT

void Admin::addCourse(string courseId, string name, int credits, int capacity) {
    // Check for duplicate course ID
    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].courseId == courseId) {
            cout << "[Admin] Course ID '" << courseId << "' already exists." << endl;
            return;
        }
    }
    if (credits <= 0)
        throw ValidationException("Credits must be greater than 0.");
    if (capacity <= 0)
        throw ValidationException("Capacity must be greater than 0.");

    courses.push_back(Course(courseId, name, credits, capacity));
    saveCourses();
    cout << "[Admin] Course added: " << name << " (" << courseId << ")" << endl;
}

void Admin::removeCourse(string courseId) {
    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].courseId == courseId) {
            cout << "[Admin] Removing course: " << courses[i].courseName << endl;
            courses.erase(courses.begin() + i);
            saveCourses();
            return;
        }
    }
    cout << "[Admin] Course ID '" << courseId << "' not found." << endl;
}

void Admin::assignProfessorToCourse(string profId, string courseId) {
    Professor* p = findProfessor(profId);
    if (p == NULL) {
        cout << "[Admin] Professor ID '" << profId << "' not found." << endl;
        return;
    }

    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].courseId == courseId) {
            courses[i].professorId = profId;
            p->assignCourse(courseId);
            saveCourses();
            saveProfessors();
            cout << "[Admin] " << p->getName()
                 << " assigned to course " << courseId << endl;
            return;
        }
    }
    cout << "[Admin] Course ID '" << courseId << "' not found." << endl;
}

void Admin::enrollStudentInCourse(string studentId, string courseId) {
    Student* s = findStudent(studentId);
    if (s == NULL) {
        cout << "[Admin] Student ID '" << studentId << "' not found." << endl;
        return;
    }

    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].courseId == courseId) {
            // Check capacity
            int enrolled = 0;
            for (int j = 0; j < (int)students.size(); j++) {
                vector<string> sc = students[j]->getCourses();
                for (int k = 0; k < (int)sc.size(); k++)
                    if (sc[k] == courseId) enrolled++;
            }
            if (enrolled >= courses[i].capacity) {
                cout << "[Admin] Course '" << courseId
                     << "' is full (" << enrolled << "/"
                     << courses[i].capacity << ")." << endl;
                return;
            }
            s->enrollCourse(courseId);
            saveStudents();
            return;
        }
    }
    cout << "[Admin] Course ID '" << courseId << "' not found." << endl;
}

void Admin::assignGrade(string studentId, string courseId, double score) {
    // Score validation (0-100) — throws ValidationException if bad
    if (score < 0.0 || score > 100.0)
        throw ValidationException(
            "Score " + to_string((int)score) + " is out of range (0-100).");

    Student* s = findStudent(studentId);
    if (s == NULL) {
        cout << "[Admin] Student ID '" << studentId << "' not found." << endl;
        return;
    }

    // Find which index in the student's course list matches
    vector<string> sc = s->getCourses();
    vector<double> sg = s->getGrades();
    bool found = false;
    for (int i = 0; i < (int)sc.size(); i++) {
        if (sc[i] == courseId) {
            sg[i] = score;
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "[Admin] Student " << studentId
             << " is not enrolled in course " << courseId << endl;
        return;
    }

    // Push the updated grade (addGrade recalcs GPA)
    s->addGrade(score);
    saveStudents();
    cout << "[Admin] Grade " << score << " assigned to "
         << s->getName() << " for course " << courseId << endl;
}

// ════════════════════════════════════════════════════════════════════════════
//  VIEW

void Admin::viewAllStudents() const {
    cout << "\n--- Students (" << students.size() << ") ---" << endl;
    if (students.empty()) { cout << "  (none)" << endl; return; }
    for (int i = 0; i < (int)students.size(); i++) {
        cout << "  " << *students[i]
             << "  | GPA: " << fixed << setprecision(2)
             << students[i]->getGPA() << endl;
    }
}

void Admin::viewAllProfessors() const {
    cout << "\n--- Professors (" << professors.size() << ") ---" << endl;
    if (professors.empty()) { cout << "  (none)" << endl; return; }
    for (int i = 0; i < (int)professors.size(); i++) {
        cout << "  " << *professors[i]
             << "  | Dept: " << professors[i]->getDepartment() << endl;
    }
}

void Admin::viewAllCourses() const {
    cout << "\n--- Courses (" << courses.size() << ") ---" << endl;
    if (courses.empty()) { cout << "  (none)" << endl; return; }
    for (int i = 0; i < (int)courses.size(); i++) {
        const Course& c = courses[i];

        // count enrolled students
        int enrolled = 0;
        for (int j = 0; j < (int)students.size(); j++) {
            vector<string> sc = students[j]->getCourses();
            for (int k = 0; k < (int)sc.size(); k++)
                if (sc[k] == c.courseId) enrolled++;
        }

        string profName = "(unassigned)";
        if (!c.professorId.empty()) {
            Professor* p = findProfessor(c.professorId);
            if (p != NULL) profName = p->getName();
        }

        cout << "  [" << c.courseId << "] " << c.courseName
             << "  | Credits: " << c.credits
             << "  | Enrolled: " << enrolled << "/" << c.capacity
             << "  | Professor: " << profName << endl;
    }
}

void Admin::viewCourseDetail(string courseId) const {
    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].courseId == courseId) {
            const Course& c = courses[i];
            cout << "\n=== Course Detail: " << c.courseName
                 << " (" << c.courseId << ") ===" << endl;
            cout << "  Credits : " << c.credits  << endl;
            cout << "  Capacity: " << c.capacity << endl;

            if (!c.professorId.empty()) {
                Professor* p = findProfessor(c.professorId);
                cout << "  Professor: "
                     << (p ? p->getName() : c.professorId) << endl;
            } else {
                cout << "  Professor: (unassigned)" << endl;
            }

            cout << "  Enrolled students:" << endl;
            int count = 0;
            for (int j = 0; j < (int)students.size(); j++) {
                vector<string> sc = students[j]->getCourses();
                vector<double> sg = students[j]->getGrades();
                for (int k = 0; k < (int)sc.size(); k++) {
                    if (sc[k] == courseId) {
                        cout << "    " << students[j]->getName()
                             << " (ID: " << students[j]->getUserId() << ")"
                             << "  Grade: ";
                        if (sg[k] < 0.0) cout << "N/A" << endl;
                        else cout << fixed << setprecision(1) << sg[k] << endl;
                        count++;
                    }
                }
            }
            if (count == 0) cout << "    (no students enrolled)" << endl;
            return;
        }
    }
    cout << "[Admin] Course '" << courseId << "' not found." << endl;
}

void Admin::saveReport() const {
    ofstream file("admin_report.txt");
    if (!file.is_open())
        throw FileException("Cannot open admin_report.txt for writing.");

    file << "=== System Report ===" << endl;
    file << "Total active users : " << User::getTotalUsers() << endl;
    file << "Students           : " << students.size()   << endl;
    file << "Professors         : " << professors.size() << endl;
    file << "Courses            : " << courses.size()    << endl;

    file << "\n-- Students --" << endl;
    for (int i = 0; i < (int)students.size(); i++) {
        file << "  ID: " << students[i]->getUserId()
             << "  | " << students[i]->getName()
             << "  | GPA: " << fixed << setprecision(2)
             << students[i]->getGPA() << endl;
    }

    file << "\n-- Professors --" << endl;
    for (int i = 0; i < (int)professors.size(); i++) {
        file << "  ID: " << professors[i]->getUserId()
             << "  | " << professors[i]->getName()
             << "  | Dept: " << professors[i]->getDepartment() << endl;
    }

    file << "\n-- Courses --" << endl;
    for (int i = 0; i < (int)courses.size(); i++) {
        const Course& c = courses[i];
        file << "  [" << c.courseId << "] " << c.courseName
             << "  Credits: " << c.credits
             << "  Capacity: " << c.capacity;
        if (!c.professorId.empty())
            file << "  Prof ID: " << c.professorId;
        file << endl;
    }

    file.close();
    cout << "[Admin] Report saved to admin_report.txt" << endl;
}

// ════════════════════════════════════════════════════════════════════════════
//  PANEL

string Admin::getRole() const { return "Admin"; }

void Admin::displayPanel() {
    int choice = 0;
    cout << "\n=== Admin Panel  |  " << name << "  |  ID: " << userId << " ===" << endl;
    cout << "  \n--- Users ---\n" << endl;
    cout << "  1.  View all students" << endl;
    cout << "  2.  View all professors" << endl;
    cout << "  3.  Create student" << endl;
    cout << "  4.  Create professor" << endl;
    cout << "  5.  Remove student by ID" << endl;
    cout << "  6.  Remove professor by ID" << endl;
    cout << "  7.  Search student (by name)" << endl;
    cout << "  8.  Search professor (by name)" << endl;
    cout << "  \n--- Courses ---\n" << endl;
    cout << "  9.  View all courses" << endl;
    cout << "  10. View course detail" << endl;
    cout << "  11. Add course" << endl;
    cout << "  12. Remove course" << endl;
    cout << "  13. Assign professor to course" << endl;
    cout << "  14. Enroll student in course" << endl;
    cout << "  15. Assign grade to student" << endl;
    cout << "  \n--- System ---\n" << endl;
    cout << "  16. Save system report to file"   << endl;
    cout << "  0.  Logout" << endl;
    cout << "Choice: ";
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        viewAllStudents();

    } else if (choice == 2) {
        viewAllProfessors();

    } else if (choice == 3) {

        string n, e, p;
        cout << "Name: ";     getline(cin, n);
        cout << "Email: ";    getline(cin, e);
        cout << "Password (min 6 chars): "; getline(cin, p);
        try {
            createStudent(n, e, p);
        } catch (ValidationException& ve) {
            cout << "[Validation Error] " << ve.what() << endl;
        } catch (FileException& fe) {
            cout << "[File Error] " << fe.what() << endl;
        }

    } else if (choice == 4) {

        string n, e, p, dept;
        cout << "Name: ";       getline(cin, n);
        cout << "Email: ";      getline(cin, e);
        cout << "Password (min 6 chars): "; getline(cin, p);
        cout << "Department: "; getline(cin, dept);
        try {
            createProfessor(n, e, p, dept);
        } catch (ValidationException& ve) {
            cout << "[Validation Error] " << ve.what() << endl;
        } catch (FileException& fe) {
            cout << "[File Error] " << fe.what() << endl;
        }

    } else if (choice == 5) {

        string id;
        cout << "Student ID: "; getline(cin, id);
        removeStudent(id);

    } else if (choice == 6) {
        string id;
        cout << "Professor ID: "; getline(cin, id);
        removeProfessor(id);

    } else if (choice == 7) {
        
        string n;
        cout << "Student name: "; getline(cin, n);
        Student* s = findStudent(n, true);
        if (s != NULL) cout << "  Found: " << *s
                            << "  | GPA: " << s->getGPA() << endl;
        else           cout << "  No student named '" << n << "' found." << endl;

    } else if (choice == 8) {
        
        string n;
        cout << "Professor name: "; getline(cin, n);
        Professor* p = findProfessor(n, true);
        if (p != NULL) cout << "  Found: " << *p
                            << "  | Dept: " << p->getDepartment() << endl;
        else           cout << "  No professor named '" << n << "' found." << endl;

    } else if (choice == 9) {
        
        viewAllCourses();

    } else if (choice == 10) {
        
        string id;
        cout << "Course ID: "; getline(cin, id);
        viewCourseDetail(id);

    } else if (choice == 11) {
        
        string cid, cname;
        int cred, cap;
        cout << "Course ID: ";   getline(cin, cid);
        cout << "Course name: "; getline(cin, cname);
        cout << "Credits: ";     cin >> cred; cin.ignore();
        cout << "Capacity: ";    cin >> cap;  cin.ignore();
        try {
            addCourse(cid, cname, cred, cap);
        } catch (ValidationException& ve) {
            cout << "[Validation Error] " << ve.what() << endl;
        } catch (FileException& fe) {
            cout << "[File Error] " << fe.what() << endl;
        }

    } else if (choice == 12) {
        
        string id;
        cout << "Course ID to remove: "; getline(cin, id);
        removeCourse(id);

    } else if (choice == 13) {
        
        string pid, cid;
        cout << "Professor ID: "; getline(cin, pid);
        cout << "Course ID: ";    getline(cin, cid);
        assignProfessorToCourse(pid, cid);

    } else if (choice == 14) {
        
        string sid, cid;
        cout << "Student ID: "; getline(cin, sid);
        cout << "Course ID: ";  getline(cin, cid);
        enrollStudentInCourse(sid, cid);

    } else if (choice == 15) {
        
        string sid, cid;
        double score;
        cout << "Student ID: "; getline(cin, sid);
        cout << "Course ID: ";  getline(cin, cid);
        cout << "Score (0-100): "; cin >> score; cin.ignore();
        try {
            assignGrade(sid, cid, score);
        } catch (ValidationException& ve) {
            cout << "[Validation Error] " << ve.what() << endl;
        }

    } else if (choice == 16) {
        try {
            saveReport();
        } catch (FileException& fe) {
            cout << "[File Error] " << fe.what() << endl;
        }
    }
    // choice 0 falls through — returns to main login loop

    if (choice != 0) displayPanel();
}
