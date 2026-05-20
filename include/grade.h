#pragma once
#include "exceptions.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class Student;
class Course;
class Grade{
    private:
        Student* student;
        Course* course;
        double score;
        char letterGrade;
        string feedback;

        void calculateLetterGrade();
    public:
        Grade(Student* student = nullptr, Course* course = nullptr, double score = 0.0);
        
        //getters
        Student* getStudent() const;
        Course* getCourse() const;
        double getScore() const;
        char getLetterGrade() const;
        string getFeedback() const;

        //setters
        void setScore(double newScore);
        void setFeedback(string feedback);
        char setLetterGrade() const;
        double setScore();

        //file handling
        void saveToFile(string filename) const;
        void loadFromFile(string filename);


        //operator overloading
        bool operator>(const Grade& other) const;
        bool operator<(const Grade& other) const;
        bool operator==(const Grade& other) const;
        bool operator!=(const Grade& other) const;
        friend ostream& operator<<(ostream& os, const Grade& g);
};