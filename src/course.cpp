#include <iostream>
#include <fstream>
#include <sstream>
#include "../include/course.h"
#include "../include/assignment.h"
#include "../include/student.h"
#include "../include/professor.h"
#include "exceptions.cpp"
using namespace std;

int Course::CourseCount = 0;

//constructors and destructors
Course::Course(string courseID, string courseName, int credits, int capacity)
    : courseID(courseID), courseName(courseName), credits(credits), capacity(capacity)    
{
    if (courseID.empty())
        throw invalid_argument("Course ID cannot be empty");
    if (courseName.empty())
        throw invalid_argument("Course name cannot be empty");
    if (credits <= 0)
        throw invalid_argument("Credits must be greater than 0");
    if (capacity <= 0)
        throw invalid_argument("Capacity must be greater than 0");
    ++CourseCount;
    cout << "[Course] Created: " << courseName
         << " (" << courseID << ")\n";
}

Course::~Course(){
    --CourseCount;
}


//getters
string Course::getCourseID() const{
    return courseID;
}

string Course::getCourseName() const{
    return courseName;
}

int Course::getCredits() const{
    return credits;
}

int Course::getCapacity() const{
    return capacity;
}

int Course::getEnrolledCount() const{
    return static_cast<int>(enrolledStudents.size());
}

vector<Student*> Course::getEnrolledStudents() const{
    return enrolledStudents;
}

vector<Assignment> Course::getAssignments(){
    return assignments;
}

const vector<Assignment> Course::getAssignments() const {
    return assignments;
}

int Course::getTotalCourses(){
    return CourseCount;
}

vector<string> Course::getMaterials() const {
    return materials;
}

vector<string> Course::getAnnouncements() const{
    return announcements;
}

Professor* Course::getProfessor() const{
    return this->professor;
}

//setters

void Course::setCourseName(string name){
    if (name.empty())
        throw invalid_argument("Course name cannot be empty");
    courseName = name;
}

void Course::setCapacity(int cap){
    if (cap < getEnrolledCount())
        throw invalid_argument(
            "New capacity cannot be less than current enrollment");
    capacity = cap;
}

void Course::setDueDate(string date){
    //no implementation yet
}

void Course::setMaxscore(double score){
    //no implementation yet
}

void Course::setProfessor(Professor* prof){
    if (!prof)
        throw invalid_argument("Professor pointer cannot be null");
    professor = prof;
    cout << "[Course] Professor set for " << courseName << endl;
}

//student management
bool Course::isFull() const{
    return getEnrolledCount() >= capacity;
}

bool Course::isEnrolled(Student* student) const{
    return find(enrolledStudents.begin(), enrolledStudents.end(), student) != enrolledStudents.end();
}

void Course::addStudent(Student* student){
    if(!student){
        throw invalid_argument("Student pointer cannot be null");
    }

    if (isFull()) {
        throw CourseFullException(
            "Course '" + courseName + 
            "' is at full capacity (" + 
            to_string(getEnrolledCount()) + "/" + 
            to_string(capacity) + ")"
        );
    }

    if(isEnrolled(student)){
        throw EnrollmentException("Student already enrolled in '" + courseName + "'");
    }

    enrolledStudents.push_back(student);
    cout << "[Course] " << student->getName() << " enrolled in " << courseName << endl;
}

void Course::removeStudent(Student* student){
    if(!student){
        throw invalid_argument("Student pointer cannot be null");
    }

    auto it = find(enrolledStudents.begin(), enrolledStudents.end(), student);
    if(it == enrolledStudents.end()){
        throw EnrollmentException("Student not enrolled in '" + courseName + "'");
    }

    enrolledStudents.erase(it);
    cout << "[Course]" << student->getName() << endl;
    cout << " removed from " << courseName << endl;
}

//content management
void Course::addAssignment(const Assignment& a){
    assignments.push_back(a);
    cout << "[Course] Assignment added to " << courseName << ": " << a.getTitle() << endl;
}

void Course::addMaterial(string material){
    if (material.empty()){
        throw invalid_argument("Material string cannot be empty");
    }
    
    materials.push_back(material);
    cout << "[Course] Material posted to "
         << courseName << ": " << material << "\n";
}

void Course::addAnnouncement(string msg){
    if (msg.empty()){
        throw invalid_argument("Announcement cannot be empty");
    }

    announcements.push_back(msg);
    cout << "[Course] Announcement in "
         << courseName << ": " << msg << "\n";
}

//File input/output
void Course::saveToFile(string filename) const{
    ofstream file(filename);
    if(!file.is_open()){
        throw FileIOException("Cannot open file for writing: " + filename);
    }

    file << courseID << endl;
    file << courseName << endl;
    file << credits << endl;
    file << capacity << endl;

    file << assignments.size() << endl;
    for(const Assignment& a : assignments){
        file << a.getTitle() << endl;
        file << a.getDueDate() << endl;
        file << a.getMaxScore() << endl;
    }

    file << materials.size() << endl;
    for (const string& m : materials){
        file << m << endl;
    }

    file << announcements.size() << endl;
    for(const string& ann : announcements){
        file << ann << endl;
    }

    file.close();
    cout << "[Course] Saved to file: " << filename << endl;
}

Course Course::loadFromFile(string filename){
    ifstream file(filename);
    if(!file.is_open()){
        throw FileIOException("Cannot open file for reading: " + filename);
    }

    string courseID, courseName;
    int credits, capacity;

    getline(file, courseID);
    getline(file, courseName);
    file >> credits >> capacity;
    file.ignore();

    Course c(courseID, courseName, credits, capacity);

    int aCount; 
    file >> aCount;
    file.ignore();
    for(int i = 0; i < aCount; ++i){
        string title, dueDate, description; //description added but not used i guess
        double maxScore;
        getline(file, title);
        file >> maxScore; file.ignore();
        c.addAssignment(Assignment(title, description, dueDate, maxScore, nullptr)); // here added too
    }

    int mCount;
    file >> mCount;
    file.ignore();
    for(int i = 0; i < mCount; ++i){
        string mat; getline(file, mat);
        c.addMaterial(mat);
    }

    int annCount;
    file >> annCount;
    file.ignore();
    for(int i = 0; i < annCount; ++i){
        string ann;
        getline(file, ann);
        c.addAnnouncement(ann);
    }

    file.close();
    cout << "[Course] Loaded from file: " << filename << endl;
    return c;
}



// operator overloading
bool Course::operator!=(const Course& other) const{
    return !(*this == other);
}

bool Course::operator==(const Course& other) const{
    return courseID == other.courseID;
}

// bool Course::operator<(const Course& other) const{

// }

// bool Course::operator>(const Course& other) const{

// }

ostream& operator<<(ostream& out, const Course& c){
    out << "[Course] " << c.courseID << endl;
    out << " - " << c.courseName << endl;
    out << " | Credits: " << c.credits << endl;
    out << " | Enrolled: " << c.getEnrolledCount() << endl;
    out << "/" << c.capacity << endl;
    return out;
}
