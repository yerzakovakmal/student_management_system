#pragma once
#include "user.h"
#include "course.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
using namespace std;

class Professor : virtual public User{
    private: 
        string employeeID;
        string department;
        vector<Course*> taughtCourses;
    public:
        Professor(string name = "UNKNOWN", string email = "unknown@example.com", string password = "password", string dept = "UNKNOWN");
        ~Professor() override;
        
        //getters
        string getEmployeeId() const;
        string getDepartment() const;
        vector<Course*> getTaughtCourses() const;
        

        // setters
        void createCourse(Course* course);
        void assignGrade(Student& student, Course& course, double score);
        void postMaterial(string material, Course& course);
        void createAssignment(Course& course, string title, string description, string dueDate, double maxScore);
        void setDepartment(string department);
        string generateReport(Course& course) const;
        
        //new addition
        void manageRoster(Course& course);

        //operator overloads
        bool operator==(const Professor& other) const;
        friend ostream& operator<<(ostream& out, const Professor& p);

        //virtual overrides from User
        string getRole() const override;
        void displayInfo() const override;
};