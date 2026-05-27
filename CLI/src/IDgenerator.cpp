#include "../include/IDgenerator.h"
#include <iostream>
using namespace std;

int IDgenerator::studentCount = 0;
int IDgenerator::professorCount = 0;
int IDgenerator::adminCount = 0;
int IDgenerator::currentYear = -1;

int IDgenerator::getCurrentYear() {
    time_t t = time(NULL);
    tm* now = localtime(&t);
    return now->tm_year % 100;// e.g. 2026 -> 26
}

void IDgenerator::resetIfNewYear() {
    int year = getCurrentYear();
    
    if (year != currentYear) {
        currentYear = year;
        studentCount = 0;
        professorCount = 0;
        adminCount = 0;
        cout << "[IDgenerator] New year (" << year << "). Counters reset." << endl;
    }
}

string IDgenerator::generateStudentID() {
    resetIfNewYear();
    ostringstream out;
    out << "S" << setw(2) << setfill('0') << currentYear << "-" << setw(4) << setfill('0') << ++studentCount;
    return out.str();
}

string IDgenerator::generateProfessorID() {
    resetIfNewYear();
    ostringstream out;
    out << "P" << setw(2) << setfill('0') << currentYear << "-" << setw(4) << setfill('0') << ++professorCount;
    return out.str();
}

string IDgenerator::generateAdminID() {
    resetIfNewYear();
    ostringstream out;
    out << "A" << setw(2) << setfill('0') << currentYear << "-" << setw(4) << setfill('0') << ++adminCount;
    return out.str();
}
