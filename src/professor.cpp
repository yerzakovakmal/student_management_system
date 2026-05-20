// Professor.cpp — implementation
#include "../include/professor.h"
#include "../include/student.h"
#include "../include/course.h"
#include "../include/assignment.h"
#include "../include/IDgenerator.h"
#include <iostream>
#include <sstream>
using namespace std;

//constructor 
Professor::Professor(string name, string email, string password, string dept)
    : User(name, email, password, "Professor", IDgenerator::ProfessorID()),
      employeeID(getEmployeeId()), department(dept)
{
    if(dept.empty()){
        throw invalid_argument("Professor department cannot be empty");
    }
    cout << "[Professor Created] " << name << " (" << employeeID << ") - Dept: " << dept << endl;
}

Professor::~Professor() {
    cout << "[Professor Deleted] " << name << " (" << employeeID << ")\n";
}

//getters
string Professor::getEmployeeId() const {
    return employeeID;
}
string Professor::getDepartment() const {
    return department;
}
vector<Course*> Professor::getTaughtCourses() const {
    return taughtCourses;
}

//setters
void Professor::setDepartment(string dept) {
    if (dept.empty())
        throw invalid_argument("Department cannot be empty");
    department = dept;
}

//core Methods
void Professor::createCourse(Course* course) {
    if (!course)
        throw invalid_argument("Course pointer is null");
    taughtCourses.push_back(course);
    cout << "Course '" << course->getCourseName()
         << "' added to professor " << name << "\n";
}

void Professor::assignGrade(Student& student, Course& course, double score) {
    if (score < 0.0 || score > 100.0){
        throw out_of_range("Score must be between 0 and 100");
    }

    student.receiveGrade(course, score);
    cout << "Grade " << score << " assigned to "
         << student.getName() << "\n";
}

void Professor::manageRoster(Course& course) {
    cout << "=== Roster: " << course.getCourseName() << " ===\n";
    for (auto* s : course.getEnrolledStudents())
        cout << "  - " << s->getName() << "\n";
}

void Professor::postMaterial(string material, Course& course) {
    course.addMaterial(material);
    cout << "Material posted to " << course.getCourseName() << "\n";
}

void Professor::createAssignment(Course& course, string title, string description, string dueDate, double maxScore) {
    if (maxScore <= 0){
        throw invalid_argument("Max score must be positive");
    }
    Assignment a(title, dueDate, description, maxScore, &course);
    course.addAssignment(a);
}

string Professor::generateReport(Course& course) const {
    ostringstream out;
    out << "Report for: " << course.getCourseName() << endl;
    out << "Students enrolled: " << course.getEnrolledStudents().size() << endl;

    for (auto* s : course.getEnrolledStudents()){
        out << "  " << s->getName() << " — GPA: " << s->calculateGPA() << endl;
    }

    return out.str();
}

// Operator Overloads
bool Professor::operator==(const Professor& other) const {
    return employeeID == other.employeeID;
}

ostream& operator<<(ostream& out, const Professor& p) {
    out << "Professor[" << p.name << " | Dept: " << p.department << " | ID: " << p.employeeID << "]" << endl;
    return out;
}

// Virtual overrides
string Professor::getRole() const { 
    return "Professor";
}

void Professor::displayInfo() const {
    cout << "=== Professor Info ===" << endl;
    cout << "Name:       " << name << endl;
    cout << "Email:      " << email << endl;
    cout << "Employee ID: " << employeeID << endl;
    cout << "Department: " << department << endl;
    cout << "Courses:    " << taughtCourses.size() << endl;
}