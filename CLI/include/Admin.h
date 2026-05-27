#pragma once
#include "user.h"
#include "student.h"
#include "professor.h"
#include "course.h"
#include <vector>

class Admin : public User {
private:
    vector<Student*> students;
    vector<Professor*> professors;
    vector<Course> courses;

    void saveStudents() const;
    void saveProfessors() const;
    void saveCourses() const;

    void loadStudents();
    void loadProfessors();
    void loadCourses();

public:
    Admin(string name, string email, string password);
    ~Admin();

    // ── user management 
    void createStudent(string name, string email, string password);
    void createProfessor(string name, string email, string password, string dept);
    void removeStudent(string userId);
    void removeProfessor(string userId);

    // Function overloading: find by ID  OR  by name
    Student* findStudent(string userId) const;
    Student* findStudent(string name, bool byName) const;
    Student* findStudentByEmail(string email) const; // Added for login fix

    Professor* findProfessor(string userId) const;
    Professor* findProfessor(string name, bool byName) const;
    Professor* findProfessorByEmail(string email) const; // Added for login fix

    // course management 
    void addCourse(string courseId, string name, int credits, int capacity, string days = "TBD", string time = "TBD");
    void removeCourse(string courseId);
    void assignProfessorToCourse(string profId, string courseId);
    void enrollStudentInCourse(string studentId, string courseId);
    void assignGrade(string studentId, string courseId, double score);

    const vector<Course>& getCourses() const { 
        return courses; // Added for Student features
    }
    const vector<Student*>& getStudents() const { 
        return students; // For login scan
    }
    const vector<Professor*>& getProfessors() const { 
        return professors; // For login scan
    }

    // ── view helpers (used by panel) 
    void viewAllStudents() const;
    void viewAllProfessors() const;
    void viewAllCourses() const;
    void viewCourseDetail(string courseId) const;

    // Save a human-readable report to admin_report.txt
    void saveReport()const;

    // Load all data from disk (called once at startup)
    void loadAllData();

    void displayPanel() override;
    string getRole() const override;
};
