#pragma once
#include "user.h"
#include <vector>

// Forward declaration
class Admin;

class Student : public User {
private:
    vector<string> enrolledCourses; // course IDs
    vector<double> grades; // one grade per course (0-100)
    double gpa;

    // Financial fields
    double annualTuition;
    double semesterTuition;
    double tuitionPaid;

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
    void setGrade(string courseId, double score); // Fix GPA Drifting: update by reference
    void recalcGPA();

    // New interfaces
    void viewTimetable(const Admin& admin) const;
    void viewTuitionStatus() const;
    void printTranscript() const;

    // Binary file I/O
    void saveToFile() const; // saves to students.dat (append)
    void writeToBinaryFull(ofstream& out) const;
    void readFromBinaryFull(ifstream& in);

    void displayPanel() override;
    void displayPanelWithAdmin(const Admin& admin); // Added for context-aware views
    string getRole() const override;
};
