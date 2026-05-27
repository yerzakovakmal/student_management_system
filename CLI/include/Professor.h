#pragma once
#include "user.h"
#include <vector>

// Forward declaration
class Admin; 

class Professor : public User {
private:
    string department;
    vector<string> taughtCourseIds;
    vector<string> assignments; // Added to store created assignments
    vector<string> announcements; // Added to store announcements

public:
    Professor(string name, string email, string password, string department);
    Professor(string userId, string name, string email, string password, string dept);
    ~Professor();

    string getDepartment() const;
    vector<string> getCourseIds() const;

    void assignCourse(string courseId);

    void viewMyStudents(const Admin& admin) const;
    void viewMyCourseDetail(const Admin& admin) const;
    void createAssignment();
    void makeAnnouncement();
    void viewAssignments() const;
    void viewAnnouncements() const;
    void gradeStudent(Admin& admin);

    // Binary file I/O
    void saveToFile() const;
    void writeToBinaryFull(ofstream& out) const;
    void readFromBinaryFull(ifstream& in);

    // Updated to take Admin reference for data coordination
    void displayPanelWithAdmin(Admin& admin); 

    void displayPanel() override;
    string getRole() const override;
};