#pragma once //without this, the header file might be included multiple times, causing redefinition errors
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
using namespace std;

class IDgenerator {
    private:
        static int studentCount;
        static int professorCount;
        static int adminCount;

        static int enrollmentYear;
        static int currentYear;

        static int getCurrentYearShort();
        static void resetIfNewYear();
    public:
        static string StudentID();
        static string ProfessorID();
        static string AdminID();
};