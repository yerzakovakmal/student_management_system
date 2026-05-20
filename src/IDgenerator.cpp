#include "../include/IDgenerator.h"
#include <sstream>
#include <iomanip>
#include <ctime>
using namespace std;

int IDgenerator::studentCount = 0;
int IDgenerator::professorCount = 0;
int IDgenerator::adminCount = 0;

int IDgenerator::currentYear =- 1;

//get current year in YY format
int IDgenerator::getCurrentYearShort() {
    time_t t = time(nullptr);
    tm* localTime = localtime(&t);
    return localTime->tm_year % 100;
}

void IDgenerator::resetIfNewYear(){
    int yearNow = getCurrentYearShort();

    if(yearNow != currentYear){
        currentYear = yearNow;
        studentCount = 0;
        professorCount = 0;
        adminCount = 0;
        cout << "[IDgenerator] New year detected. Counters reset." << endl;
    }
}
string IDgenerator::StudentID() {
    resetIfNewYear();
    
    ostringstream out;

    out << "U" << setw(2) << setfill('0') << currentYear << setw(4) << setfill('0') << studentCount++;

    return out.str();
}

string IDgenerator::ProfessorID() {
    resetIfNewYear();
    
    ostringstream out;

    out << "P" << setw(2) << setfill('0') << currentYear << setw(4) << setfill('0') << professorCount++;

    return out.str(); 
}

string IDgenerator::AdminID() {
    resetIfNewYear();

    ostringstream out;

    out << "A" << setw(2) << setfill('0') << currentYear << setw(4) << setfill('0') << adminCount++;

    return out.str(); 
}