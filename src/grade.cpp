#include <iostream>
#include <iomanip>
#include <sstream>
#include "../include/grade.h"
#include "../include/student.h"
#include "../include/course.h"
using namespace std;

void Grade::calculateLetterGrade(){
    if(score <= 90.0)
        letterGrade = 'A';
    else if(score >= 80.0)
        letterGrade = 'B';
    else if(score >= 70.0)
        letterGrade = 'C';
    else if(score >= 60)
        letterGrade = 'D';
    else
        letterGrade = 'F';
}

Grade::Grade(Student* student, Course* course, double score)
    : student(student), course(course), score(score), letterGrade('F'), feedback(""){
        if(!student){
            throw invalid_argument("Grade: student pointer cannot be null");
        }

        if(score < 0.0 || score > 100.0){
            throw GradeOutOfRangeException("Score " + to_string(score) + " is out of range(0-100)");
        }

        calculateLetterGrade();
    }

//getters
Student* Grade::getStudent() const {
    return student;
}

Course* Grade::getCourse() const{
    return course;
}

double Grade::getScore() const{
    return score;
}

char Grade::getLetterGrade() const{
    return letterGrade;
}

string Grade::getFeedback() const{
    return feedback;
}

//setters

void Grade::setScore(double newScore){
    if(newScore  < 0.0 || newScore > 100.0){
        throw GradeOutOfRangeException("Score " + to_string(newScore) + " is out of range (0–100)");
    }

    double old = score;
    score = newScore;
    calculateLetterGrade();

    cout << "[Grade] Score updated: " << fixed << setprecision(1) << old << " -> " << newScore << " (grade" << letterGrade << ")" << endl;
}

void Grade::setFeedback(string fb){
    if (fb.empty())
        throw invalid_argument("Feedback cannot be empty");
    feedback = fb;
}

//file input output
void Grade::saveToFile(string filename) const{
    ofstream file(filename, ios::app);   // append — one file per course
    if (!file.is_open())
        throw FileIOException(
            "Cannot open file for writing: " + filename);

    // Format: studentId | courseId | score | letter | feedback
    string studentId;
    if (student) {
        studentId = student->getUserId();
    } else {
        studentId = "UNKNOWN";
    }

    string courseId;
    if (course) {
        courseId = course->getCourseID();
    } else {
        courseId = "UNKNOWN";
    }

    string feedbackText;
    if (feedback.empty()) {
        feedbackText = "none";
    } else {
        feedbackText = feedback;
    }

    file << studentId << "|"
        << courseId  << "|"
        << fixed << setprecision(2) << score << "|"
        << letterGrade << "|"
        << feedbackText << endl;

    file.close();
}

void Grade::loadFromFile(string filename){
    ifstream file(filename);
    if(!file.is_open()){
        throw FileIOException("Cannot open file for recording: " + filename);
    }

    string line;
    while(getline(file, line)){
        istringstream iss(line);
        string studentID, courseID, scoreStr, letterStr, fb;

        getline(iss, studentID, '|');
        getline(iss, courseID, '|');
        getline(iss, scoreStr,  '|');
        getline(iss, letterStr, '|');
        getline(iss, fb, '|');

        score = stod(scoreStr);

        if(letterStr.empty()){
            letterGrade = 'F';
        } else{
            letterGrade = letterStr[0];
        }

        if (fb == "none") {
            feedback = "";
        } else {
            feedback = fb;
        }
    }
    file.close();
}


//operator overloading
bool Grade::operator>(const Grade& other) const{
    return score > other.score;
}

bool Grade::operator<(const Grade& other) const{
    return score < other.score;
}

bool Grade::operator==(const Grade& other) const{
    return student == other.student && course  == other.course;
}

bool Grade::operator!=(const Grade& other) const{
    return !(*this == other);
}

ostream& operator<<(ostream& out, const Grade& g) {
    string studentName;

    if (g.student) {
        studentName = g.student->getName();
    } else {
        studentName = "";
    };

    string courseName;
    if(g.course){
        courseName = g.course->getCourseName();
    } else {
        courseName = "";
    }

    out << "[Grade] " << endl;
    out  << studentName << endl;
    out << " | Course: " << courseName;
    out << " | Score: " << fixed << setprecision(1) << g.score;
    out << " | Grade: " << g.letterGrade;

    if(!g.feedback.empty()){
        out << " | Feedback: " << g.feedback;
    }

    return out;
}
