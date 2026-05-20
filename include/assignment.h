#pragma once
#include <iostream>
#include <string>
using namespace std;

class Course;

class Assignment{
    private:
        int assignmentID;
        string title;
        string description;
        string dueDate;
        double maxScore;
        Course* course;

        static int nextID;
    public:
        Assignment(string title = "UNKNOWN", string description = "UNKNOWN", string dueDate = "01-01-2023", double maxScore = 100.0, Course* course = nullptr);

        //getters
        int getAssignmentID() const;
        string getDescription() const;
        string getTitle() const;
        string getDueDate() const;
        double getMaxScore() const;
        Course* getCourse() const;

        //setters
        void setTitle(string title);
        void setDescription(string description);
        void setDueDate(string date);
        void setMaxScore(double score);


        bool isOverdue(string todayDate) const;
        static int getTotalAssignments();

        //operator overloading
        friend ostream& operator<<(ostream& out, const Assignment& a);
        bool operator==(const Assignment& other) const;
        bool operator!=(const Assignment& other) const;
        bool operator<(const Assignment& other) const;
        bool operator>(const Assignment& other) const;
};