#pragma once
#include "User.h"
#include <vector>

class Student : public User {
private:
    vector<string> enrolledCourses; // course IDs
    vector<double> grades; // one grade per course (0-100)
    double gpa;

public:
    Student(string name, string email, string password);

    // Constructor used when loading from binary (ID already known)
    Student(string userId, string name, string email, string password);
    ~Student();

    vector<string> getCourses() const;
    vector<double> getGrades() const;
    double getGPA() const;

    void enrollCourse(string courseId);
    void addGrade(double score); // validated: 0-100
    void recalcGPA();

    // Binary file I/O
    void saveToFile() const; // saves to students.dat (append)
    void writeToBinaryFull(ofstream& out) const;
    void readFromBinaryFull(ifstream& in);

    void displayPanel() override;
    string getRole() const override;
};
