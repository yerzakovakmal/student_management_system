#pragma once
#include "User.h"
#include <vector>

class Professor : public User {
private:
    string department;
    vector<string> taughtCourseIds;// course IDs assigned by admin

public:
    Professor(string name, string email, string password, string department);

    // Constructor used when loading from binary (ID already known)
    Professor(string userId, string name, string email, string password, string dept);
    ~Professor();

    string getDepartment() const;
    vector<string> getCourseIds()  const;

    void assignCourse(string courseId);

    // Binary file I/O
    void saveToFile() const;
    void writeToBinaryFull(ofstream& out) const;
    void readFromBinaryFull(ifstream& in);

    void displayPanel() override;
    string getRole() const override;
};
