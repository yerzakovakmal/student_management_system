#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "user.h"
#include "course.h"
#include "grade.h"
#include "exceptions.h"
using namespace std;

class Course;
class Assignment;
class Student : virtual public User{
    private:
        double gpa;
        vector<Course*> enrolledCourses;
        vector<Grade> grades;
        int semester;

    public:
        Student(string name = "UNKNOWN", string email = "unknown@example.com", string password = "password");
        ~Student() override;


        //getters
        double getGPA() const;
        vector<Course*> getEnrolledCourses() const;
        vector<Grade> getGrades() const;
        int getCourseCount() const;

        //enrollment
        void enrollCourse(Course& course);
        void dropCourse(Course& course);
        bool isEnrolledIn(Course& course) const;

        //academic
        double calculateGPA();
        void receiveGrade(Course& course, double score);
        void submitAssignment(Assignment& assignment);
        void viewGrades() const;
        string viewTranscript() const;

        //operator overloading
        friend ostream& operator<<(ostream& out, const Student& c);
        bool operator==(const Student& other) const;
        bool operator!=(const Student& other) const;

        string getRole() const override;
        void displayInfo() const override;
};