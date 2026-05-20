// main.cpp — University Management System demo
// Exercises every major class: Admin, Professor, Student, Course, Assignment, Grade
// Binary file I/O for Course and Grade is demonstrated at the end.

#include <iostream>
#include <vector>
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

static void separator(const string& title) {
    cout << "\n╔══════════════════════════════════════════════╗\n"
         << "  " << title << "\n"
         << "╚══════════════════════════════════════════════╝\n";
}


int main() {

    // ── 0. Configure academic year ────────────────────────────────────────────
    separator("0. IDgenerator — initialized");
    // Enrollment year is managed internally by IDgenerator.

    // ── 1. Create Admin ───────────────────────────────────────────────────────
    separator("1. Admin creation");
    Admin* admin = new Admin("Dr. Karimov", "karimov@iut.uz", "securePass1", "Registry");
    admin->displayInfo();

    // ── 2. Admin creates users ────────────────────────────────────────────────
    separator("2. Admin creates Professor and Students");

    Professor* prof = admin->createProfessor(
        "Prof. Aliyev", "aliyev@iut.uz", "profPass1", "Computer Science");

    Student* s1 = admin->createStudent("Akmal Toshmatov", "akmal@iut.uz",   "pass123");
    Student* s2 = admin->createStudent("Zulfiya Nazarova","zulfiya@iut.uz", "pass456");
    Student* s3 = admin->createStudent("Bobur Yusupov",   "bobur@iut.uz",   "pass789");

    cout << "\nTotal users registered: " << User::getTotalUsers() << "\n";

    // ── 3. Create courses ─────────────────────────────────────────────────────
    separator("3. Course creation");
    Course* cpp   = new Course("CS101", "C++ Programming", 3, 2);   // cap=2 to test overflow
    Course* calc  = new Course("MATH201","Calculus II",    4, 30);
    Course* phys  = new Course("PHY102", "Physics 2",     3, 30);

    cout << "\nTotal courses: " << Course::getTotalCourses() << "\n";

    // ── 4. Assign professor to courses ────────────────────────────────────────
    separator("4. Admin assigns professor to courses");
    admin->assignProfessorToCourse(*prof, *cpp);
    admin->assignProfessorToCourse(*prof, *calc);

    // ── 5. Student enrollment ─────────────────────────────────────────────────
    separator("5. Student enrollment");
    s1->enrollCourse(*cpp);
    s2->enrollCourse(*cpp);

    // Test CourseFullException — cpp has capacity 2
    try {
        s3->enrollCourse(*cpp);   // should throw
    } catch (const CourseFullException& e) {
        cout << "[CAUGHT CourseFullException] " << e.what() << "\n";
    }

    // Test duplicate enrollment
    try {
        s1->enrollCourse(*cpp);   // should throw
    } catch (const EnrollmentException& e) {
        cout << "[CAUGHT EnrollmentException] " << e.what() << "\n";
    }

    s1->enrollCourse(*calc);
    s2->enrollCourse(*calc);
    s3->enrollCourse(*calc);
    s1->enrollCourse(*phys);

    // ── 6. Assignments ────────────────────────────────────────────────────────
    separator("6. Professor creates assignments");
    prof->createAssignment(*cpp,  "Lab 1 - Hello World", "Write your first C++ program",
                           "15-05-2025", 100.0);
    prof->createAssignment(*cpp,  "Lab 2 - OOP Basics",  "Implement a class hierarchy",
                           "30-05-2025", 100.0);
    prof->createAssignment(*calc, "HW 1 - Series",       "Convergence tests for series",
                           "20-05-2025", 50.0);

    cout << "\nTotal assignments created: " << Assignment::getTotalAssignments() << "\n";

    // Student submits an assignment
    separator("6b. Student submits assignment");
    vector<Assignment> cppAssignments = cpp->getAssignments();
    if (!cppAssignments.empty())
        s1->submitAssignment(cppAssignments[0]);

    // ── 7. Materials and announcements ────────────────────────────────────────
    separator("7. Professor posts material and announcement");
    prof->postMaterial("https://cppreference.com — STL reference", *cpp);
    cpp->addAnnouncement("Midterm exam is on 10-06-2025. Bring student ID.");

    // ── 8. Grading ────────────────────────────────────────────────────────────
    separator("8. Professor assigns grades");
    prof->assignGrade(*s1, *cpp,  92.0);
    prof->assignGrade(*s2, *cpp,  78.5);
    prof->assignGrade(*s1, *calc, 85.0);
    prof->assignGrade(*s2, *calc, 60.0);
    prof->assignGrade(*s3, *calc, 45.0);   // fails — F grade

    // Test out-of-range grade
    try {
        prof->assignGrade(*s1, *calc, 110.0);
    } catch (const out_of_range& e) {
        cout << "[CAUGHT out_of_range] " << e.what() << "\n";
    }

    // ── 9. View grades & transcripts ──────────────────────────────────────────
    separator("9. Student grade views");
    s1->viewGrades();
    cout << "\n" << s1->viewTranscript();
    cout << "\n" << s2->viewTranscript();

    // ── 10. Professor roster & report ─────────────────────────────────────────
    separator("10. Professor manages roster & generates report");
    prof->manageRoster(*cpp);
    cout << "\n" << prof->generateReport(*calc);

    // ── 11. User login / profile ──────────────────────────────────────────────
    separator("11. User login and profile update");
    try {
        s1->login("akmal@iut.uz", "pass123");
    } catch (const runtime_error& e) {
        cout << "[CAUGHT] " << e.what() << "\n";
    }

    // Bad login attempt
    try {
        s1->login("akmal@iut.uz", "wrongPassword");
    } catch (const runtime_error& e) {
        cout << "[CAUGHT bad login] " << e.what() << "\n";
    }

    s1->updateProfile("Akmal T. Toshmatov", "akmal.t@iut.uz");
    s1->changePassword("pass123", "newSecure99");

    // ── 12. Operator overloads demo ───────────────────────────────────────────
    separator("12. Operator overload demo");
    cout << *s1  << "\n\n";
    cout << *s2  << "\n\n";
    cout << *prof << "\n";
    cout << *cpp  << "\n";
    cout << (*s1 == *s2 ? "s1 == s2" : "s1 != s2") << "\n";
    cout << (*cpp == *calc ? "cpp == calc" : "cpp != calc") << "\n";

    // Grade comparison
    vector<Grade> allGrades = s1->getGrades();
    if (allGrades.size() >= 2) {
        const Grade& g1 = allGrades[0];
        const Grade& g2 = allGrades[1];
        cout << "Grade comparison: g1 "
             << (g1 > g2 ? ">" : g1 < g2 ? "<" : "==")
             << " g2\n";
        cout << g1 << "\n";
    }

    // ── 13. Admin permission management ──────────────────────────────────────
    separator("13. Admin permission management");
    admin->addPermission("export_data");
    admin->addPermission("export_data");   // duplicate — should warn, not throw
    admin->deletePermission("export_data");

    try {
        admin->deletePermission("nonexistent_perm");
    } catch (const invalid_argument& e) {
        cout << "[CAUGHT] " << e.what() << "\n";
    }

    // ── 14. Admin system report ───────────────────────────────────────────────
    separator("14. Admin generates system report");
    vector<Student*>   students   = {s1, s2, s3};
    vector<Professor*> professors = {prof};
    vector<Course*>    courses    = {cpp, calc, phys};
    cout << admin->generateReport(students, professors, courses);

    // ── 15. findByID demo ─────────────────────────────────────────────────────
    separator("15. findStudentByID");
    try {
        string studentId = s2->getUserId();
        Student* found = admin->findStudentByID(students, studentId);
        cout << "Found: " << found->getName() << " (" << found->getUserId() << ")\n";
    } catch (const InvalidUserException& e) {
        cout << "[CAUGHT] " << e.what() << "\n";
    }

    try {
        string missingId = "U999999";
        admin->findStudentByID(students, missingId);   // not found
    } catch (const InvalidUserException& e) {
        cout << "[CAUGHT not found] " << e.what() << "\n";
    }

    // ── 16. Binary File I/O — Course ─────────────────────────────────────────
    separator("16. Binary File I/O — Course save / load");
    const string courseFile = "calc_course.bin";
    calc->saveToFile(courseFile);

    try {
        Course loadedCalc = Course::loadFromFile(courseFile);
        cout << loadedCalc;
    } catch (const FileIOException& e) {
        cout << "[FileIOException] " << e.what() << "\n";
    }

    // ── 17. Binary File I/O — Grade ──────────────────────────────────────────
    separator("17. Binary File I/O — Grade save / load");
    const string gradeFile = "s1_grades.bin";

    // Save all of s1's grades
    for (const Grade& g : s1->getGrades())
        g.saveToFile(gradeFile);

    // Load back the first record into a standalone Grade object
    // (Grade needs non-null student — we pass s1 and course as context)
    try {
        // Construct a placeholder grade and overwrite its fields from file
        Grade loaded(s1, cpp, 0.0);   // dummy score — will be overwritten
        loaded.loadFromFile(gradeFile);
        cout << "Loaded grade: " << loaded << "\n";
    } catch (const FileIOException& e) {
        cout << "[FileIOException] " << e.what() << "\n";
    }

    // ── 18. Drop course ───────────────────────────────────────────────────────
    separator("18. Student drops a course");
    s1->dropCourse(*phys);

    try {
        s1->dropCourse(*phys);   // not enrolled anymore — should throw
    } catch (const EnrollmentException& e) {
        cout << "[CAUGHT] " << e.what() << "\n";
    }

    // ── 19. Admin deletes a user ──────────────────────────────────────────────
    separator("19. Admin deletes a user");
    cout << "Total users before delete: " << User::getTotalUsers() << "\n";
    User* s3Ptr = s3;
    admin->deleteUser(s3Ptr);   // sets s3Ptr to nullptr
    cout << "Total users after delete : " << User::getTotalUsers() << "\n";
    // Note: s3 pointer is now dangling — don't use it after this point
    s3 = nullptr;

    // ── 20. Admin removes a course ────────────────────────────────────────────
    separator("20. Admin removes a course");
    cout << "Total courses before remove: " << Course::getTotalCourses() << "\n";
    admin->removeCourse(phys);   // sets phys to nullptr
    cout << "Total courses after remove : " << Course::getTotalCourses() << "\n";

    // ── Cleanup — delete remaining heap objects ────────────────────────────────
    separator("Cleanup");
    // Students first (they don't own any courses)
    delete s1;
    delete s2;
    // Courses
    delete cpp;
    delete calc;
    // Professor and admin last
    delete prof;
    delete admin;

    cout << "\nTotal users remaining: " << User::getTotalUsers()    << "\n";
    cout << "Total courses remaining: " << Course::getTotalCourses() << "\n";
    cout << "\n[main] Done. All resources freed.\n";
    return 0;
}









---------------------------------