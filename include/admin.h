#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "user.h"
#include "student.h"
#include "professor.h"
#include "course.h"

using namespace std;

class Admin : virtual public User{
    private:
        string adminDepartment;
        vector<string> permissions;
        static int nextOrderNum;
        static int enrollmentYear;
        static string generateID();

    public:
        Admin(string name = "UNKNOWN", string email = "unknown@example.com", string password = "password", string dept = "UNKNOWN");
        
        ~Admin() override;

        //getters
        string getAdminDepartment() const;
        vector<string> getPermissions() const;

        //setters
        void setAdminDepartment(string dept);

        // permission management
        bool hasPermission(string perm) const;
        void addPermission(string perm);
        void deletePermission(string perm);


        //User creation
        Student* createStudent(string name, string email, string password);
        Professor* createProfessor(string name, string email, string password, string dept);

        //User management
        void deleteUser(User*& userPtr);
        void viewAllStudents(const vector<Student*>& students) const;
        void viewAllProfessors(const vector<Professor*>& profs) const;

        Student* findStudentByID(const vector<Student*>& students, string& userId) const;
        Professor* findProfessorByID(const vector<Professor*>& profs, string& userId) const;

        //Course management
        void assignProfessorToCourse(Professor& prof, Course& course);
        void removeCourse(Course*& coursePtr);

        string generateReport(const vector<Student*>& students, const vector<Professor*>& professors, const vector<Course*>& courses) const;


        //Operator Overloading
        friend ostream& operator<<(ostream& out, const Admin& a);
        bool operator==(const Admin& other) const;
        bool operator!=(const Admin& other) const;

        // pure virtual function (User)
        string getRole() const override;
        void displayInfo() const override;
};