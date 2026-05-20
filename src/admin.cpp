#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "../include/admin.h"
#include "../include/IDgenerator.h"
using namespace std;

string Admin::generateID() {
    return IDgenerator::AdminID();
}

//constructor & destructor
Admin::Admin(string name, string email, string password, string dept) : User(name, email, password, "Admin", IDgenerator::AdminID()), adminDepartment(dept){
    if(dept.empty()){
        throw invalid_argument("Admin department cannot be empty");
    }
    userId = Admin::generateID();

    permissions = {"view_users", "create_user", "delete_user", "manage_courses"};

    cout << "[Admin Created] " << name << " (" << userId << ") - Dept: " << dept << endl;
}

Admin::~Admin(){}

//getters
string Admin::getAdminDepartment() const{
    return adminDepartment;
}

vector<string> Admin::getPermissions() const{
    return permissions;
}

//setters
void Admin::setAdminDepartment(string dept){
    if(dept.empty()){
        throw invalid_argument("Department cannot be empty");
    }
    adminDepartment = dept;
}

//permission management
bool Admin::hasPermission(string perm) const{
    return find(permissions.begin(), permissions.end(), perm) != permissions.end();
}

void Admin::addPermission(string perm){
    if (hasPermission(perm)) {
        cout << "[Admin] Permission already exists for " << name << ": " << perm << endl;
        return;
    }
    if(perm.empty()){
        throw invalid_argument("Permission cannot be empty");
    }
    permissions.push_back(perm);
    cout << "[Admin] Permission added to " << name << ": " << perm << endl;
}

void Admin::deletePermission(string perm){
    auto it = find(permissions.begin(), permissions.end(), perm);
    if (it == permissions.end()){
        throw invalid_argument("Permission '" + perm + "' not found");
    }
    permissions.erase(it);
    cout << "[Admin] Permission deleted from " << name << ": " << perm << endl;
}

//User creation
Student* Admin::createStudent(string name, string email, string password){
    if(!hasPermission("create_user")){
        throw InvalidUserException("Admin '" + this->name + "' has no 'create_user' permission");
    }

    Student* s = new Student(name, email, password);
    cout << "[Admin] Student Created: " << name << " (" << s->getUserId() << ")" << endl;
    return s;
}

Professor* Admin::createProfessor(string name, string email, string password, string dept){
    if(!hasPermission("create_user")){
        throw InvalidUserException("Admin '" + this->name + "' has no 'create_user' permission");
    }
    if(dept.empty()){
        throw InvalidUserException("Professor department cannot be empty");
    }

    //creation
    Professor* p = new Professor(name, email, password, dept); //there's empId in constructor, needs to be removed i guess
    cout << "[Admin] Professor created: " << name << " (" << p->getUserId() << ") - Dept: " << dept << endl;
    return p;
}

void Admin::deleteUser(User*& userPtr){
    if(!userPtr){
        throw invalid_argument("Cannot delete a null user pointer");
    }
    if(!hasPermission("delete_user")){
        throw InvalidUserException("Admin '" + this->name + "' has no 'delete_user' permission");
    }

    delete userPtr;
    userPtr = nullptr;
}

void Admin::viewAllStudents(const vector<Student*>& students) const {
     cout << "\n── All Students (" << students.size() << ") ──\n";
    if (students.empty()) { cout << "  (none)\n"; return; }
    for (const Student* s : students)
        cout << "  " << s->getUserId()
             << "  " << s->getName()
             << "  <" << s->getEmail() << ">\n";
}

void Admin::viewAllProfessors(const vector<Professor*>& profs) const{
    cout << "\n── All Professors (" << profs.size() << ") ──\n";
    if (profs.empty()) { cout << "  (none)\n"; return; }
    for (const Professor* p : profs)
        cout << "  " << p->getUserId()
             << "  " << p->getName()
             << "  Dept: " << p->getDepartment() << "\n";
}

Student* Admin::findStudentByID(const vector<Student*>& students, string& userId) const {
    for (Student* s : students){
        if (s && static_cast<User*>(s)->getUserId() == userId){
            return s;
        }
    }
    throw InvalidUserException("Student not found: " + userId);
}

Professor* Admin::findProfessorByID(const vector<Professor*>& profs, string& userId) const {
    for (Professor* p : profs)
        if (p && static_cast<User*>(p)->getUserId() == userId) return p;
    throw InvalidUserException("Professor not found: " + userId);
}

//Course Management
void Admin::assignProfessorToCourse(Professor& prof, Course& course){
    if (!hasPermission("manage_courses"))
        throw InvalidUserException(
            "Admin '" + name + "' lacks 'manage_courses' permission");

    prof.createCourse(&course);
    cout << "[Admin] " << prof.getName()
         << " assigned to course: "
         << course.getCourseName() << "\n";
}

void Admin::removeCourse(Course*& coursePtr){
    if (!coursePtr)
        throw invalid_argument("Cannot remove a null course pointer");
    if (!hasPermission("manage_courses"))
        throw InvalidUserException(
            "Admin '" + name + "' lacks 'manage_courses' permission");

    cout << "[Admin] Removing course: "
         << coursePtr->getCourseName() << "\n";
    delete coursePtr;
    coursePtr = nullptr;
}

string Admin::generateReport(const vector<Student*>& students, const vector<Professor*>& professors, const vector<Course*>& courses) const{
    ostringstream out;
    out << "╔══════════════════════════════════╗\n"
        << "  SYSTEM REPORT — IUT\n"
        << "  Generated by: " << name << " (" << adminDepartment << ")\n"
        << "╚══════════════════════════════════╝\n";

    out << "Total users   : " << User::getTotalUsers()  << endl;
    out << "  Students    : " << students.size()         << endl;
    out << "  Professors  : " << professors.size()       << endl;
    out << "Total courses : " << courses.size()          << endl;
    out << "Course count  : " << Course::getTotalCourses() << endl;

    out << "── Courses ─────────────────────────\n";
    for (const Course* c : courses)
        out << "  " << c->getCourseID()
            << "  " << c->getCourseName()
            << "  [" << c->getEnrolledStudents().size()
            << "/"  << c->getCapacity() << "]\n";

    return out.str();
}

//operator overloading 
bool Admin::operator==(const Admin& other) const{
    return userId == other.userId;
}

bool Admin::operator!=(const Admin& other) const{
    return !(*this == other);
}

string Admin::getRole() const{ 
    return "Admin";
}

void Admin::displayInfo() const{
    cout << "=== Admin Info ===" << endl;
    cout << "Name: " << name << endl;
    cout << "Email: " << email << endl;
    cout << "ID: " << userId << endl;
    cout << "Department: " << adminDepartment << endl;
    cout << "Permissions: " << endl;
    for(const string& p : permissions) {
        cout << "  - " << p << endl;
    }
}