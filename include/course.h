#pragma once
#include <iostream>
#include <string>
using namespace std;

class Student;
class Professor;
class Assignment;
class Course{
    private:
        string courseID;
        string courseName;
        int credits;
        int capacity;
        vector<Student*> enrolledStudents;
        vector<Assignment> assignments;
        vector<string> materials;
        vector<string> announcements;
        Professor* professor;
        static int CourseCount;
    public:
        Course(string courseID = "UNKNOWN", string courseName = "UNKNOWN", int credits = 3, int capacity = 30);
        ~Course();

        //getters
        string getCourseID() const;
        string getCourseName() const;
        int getCredits() const;
        int getCapacity() const;
        int getEnrolledCount() const;
        vector<Student*> getEnrolledStudents() const;
        vector<Assignment> getAssignments();
        const vector<Assignment> getAssignments() const;
        vector<string> getMaterials() const;
        vector<string> getAnnouncements() const;
        Professor* getProfessor() const;
        static int getTotalCourses();

        //setters
        void setCourseName(string name);
        void setCapacity(int capacity);
        void setDueDate(string date);
        void setMaxscore(double score);
        void setProfessor(Professor* prof);

        //student management
        void addStudent(Student* student);
        void removeStudent(Student* student);
        bool isEnrolled(Student* student) const;
        bool isFull() const;

        //content management
        void addAssignment(const Assignment& a);
        void addMaterial(string material);
        void addAnnouncement(string msg);

        //file input/outpu
        void saveToFile(string filename) const;
        static Course loadFromFile(string filename);

        //operator overload
        friend ostream& operator<<(ostream& out, const Course& c);
        bool operator==(const Course& other) const;
        bool operator!=(const Course& other) const;
        // bool operator<(const Course& other) const;
        // bool operator>(const Course& other) const;
};