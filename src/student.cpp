#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "../include/course.h"
#include "../include/assignment.h"
#include "../include/student.h"
#include "../include/IDgenerator.h"
using namespace std;


//constructor
Student::Student(string name, string email, string password)
    : User(name, email, password, "Student", IDgenerator::StudentID()), gpa(0.0)
{
    userId = IDgenerator::StudentID();
    cout << "[Student] Account Created: " << name << " (" << userId << ")" << endl;
}

//destructor
Student::~Student(){}


//getters
double Student::getGPA() const{
    return gpa;
}

vector<Course*> Student::getEnrolledCourses() const{
    return enrolledCourses;
}

vector<Grade> Student::getGrades() const{
    return grades;
}

int Student::getCourseCount() const{
    return static_cast<int>(enrolledCourses.size());
}

//enrollment
bool Student::isEnrolledIn(Course& course) const{
    return find(enrolledCourses.begin(), enrolledCourses.end(), &course) != enrolledCourses.end();
}

void Student::enrollCourse(Course& course) {
    if(isEnrolledIn(course)){
        throw EnrollmentException("Already enrolled in '" + course.getCourseName() + "'");
    }

    if(course.isFull()){
        throw CourseFullException("Course '" + course.getCourseName() + "' is full");
    }

    course.addStudent(this);
    enrolledCourses.push_back(&course);
    cout << "[Student] " << name << " enrolled in '" << course.getCourseName() << "'" << endl;
}

void Student::dropCourse(Course& course) {
    if (!isEnrolledIn(course)){
        throw EnrollmentException("Not enrolled in '" + course.getCourseName() + "'");
    }

    //Remove from student side
    auto it = find(enrolledCourses.begin(), enrolledCourses.end(), &course);
    enrolledCourses.erase(it);

    //Remove from course side
    course.removeStudent(this);

    cout << "[Student] " << name << " dropped '" << course.getCourseName() << "'\n";
}

//academic
double Student::calculateGPA(){
    //validation
    if (grades.empty()){ 
        gpa = 0.0;
        return gpa;
    }

    double total = 0.0;

    for (const Grade& g : grades){
        total += g.getScore();
    }
    
    gpa = total / static_cast<double>(grades.size());
    return gpa;
}

void Student::receiveGrade(Course& course, double score){
    if (!isEnrolledIn(course))
        throw EnrollmentException(
            "Cannot receive grade — not enrolled in '" +
            course.getCourseName() + "'");
    if (score < 0.0 || score > 100.0){
        throw GradeOutOfRangeException("Score " + to_string(score) + " is out of range (0-100)");
    }

    grades.emplace_back(this, &course, score);
    calculateGPA();

    cout << "[Student] Grade recorded for " << name << " in '"   << course.getCourseName() << "': "     << score << " (GPA now: " << fixed << setprecision(2) << gpa << ")" << endl;
}

void Student::submitAssignment(Assignment& assignment){
    Course* c = assignment.getCourse();
    if (c && !isEnrolledIn(*c)){
        throw EnrollmentException("Cannot submit — not enrolled in the assignment's course");
    }
    cout << "[Student] " << name << " submitted: '" << assignment.getTitle() << "'" << endl;
}

void Student::viewGrades() const{
    cout << "\n── Grades for " << name << " ──" << endl;
    if (grades.empty()) {
        cout << "  (no grades recorded yet)" << endl;
        return;
    }

    for (const Grade& g : grades){
        cout << " " << g << endl; //uses Grade's :: operator
    }
    cout << "Current GPA: " << fixed << setprecision(2) << gpa << endl;
}

string Student::viewTranscript() const{
    ostringstream out;
    out << " OFFICIAL TRANSCRIPT - IUT " << endl;
    out << "Name  : " << name << endl;
    out << "ID    : " << userId << endl;
    out << "Email : " << email << endl;
    out << "GPA   :" << fixed << setprecision(2) << gpa << endl;
    out << "────────────────────────────────────" << endl;


    if(grades.empty()) {
        out << " (no grades on record)" << endl;
    } else {
        out << left << setw(30) << "Course" << setw(8) << "Score" << setw(6) << "Grade" << endl;
        for (const Grade& g : grades){
            string CourseName = g.getCourse() ? g.getCourse() ->getCourseName() : "Unknown Course";

            out << left << setw(30) << CourseName << setw(8) << fixed << setprecision(1) << g.getScore() << setw(6) << g.getLetterGrade() << endl;
        }
    }
    out << "────────────────────────────────────" << endl;
    out << "Course enrolled : " << enrolledCourses.size() << endl;

    return out.str();
}

//operator oveloading
bool Student::operator==(const Student& other) const{
    return userId == other.userId;
}

bool Student::operator!=(const Student& other) const{
    return !(*this == other);
}

ostream& operator<<(ostream& out, const Student& s){
    out << "[Student] " << s.name << endl;
    out << " | ID: " << s.userId << endl;
    out << " | GPA: " << fixed << setprecision(2) << s.gpa << endl;
    out << " | Courses: " << s.enrolledCourses.size() << endl;
    out << " | Grades: " << s.grades.size();
    return out;
}

string Student::getRole() const {
    return "Student";
}

void Student::displayInfo() const {
    cout << "=== Student Info ===" << endl;
    cout << "Name    : " << name << endl;
    cout << "Email   : " << email << endl;
    cout << "ID      : " << userId << endl;
    cout << "GPA     : " << fixed << setprecision(2) << gpa << endl;
    cout << "Courses : " << enrolledCourses.size() << endl;
    cout << "Grades  : " << grades.size() << endl;

    if(!enrolledCourses.empty()) {
        cout << "Enrolled in: ";
        for(const Course* c : enrolledCourses){
            cout << " - " << c->getCourseName() << " (" << c->getCourseID()  << ")" << endl;
        }
    }
}