#pragma once
#include <string>
#include <vector>
#include <fstream>
using namespace std;

//Course record owned and managed by Admin
struct Course {
    string courseId;
    string courseName;
    string professorId; // which professor teaches it (empty = unassigned)
    int capacity;
    int credits;
    string daysOfWeek;  //Mon,Wed,Fri
    string timeSlot;    //09:00 - 10:30

    Course();
    Course(string courseId, string name, int credits, int capacity, string days = "TBD", string time = "TBD");
};

// Free helpers for binary serialisation of a single Course
void writeCourseBinary(ofstream& out, const Course& c);
void readCourseBinary(ifstream& in, Course& c);
