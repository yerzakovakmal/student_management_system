#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
using namespace std;

// Student  -> S26-0001, S26-0002 ...
// Professor-> P26-0001, P26-0002 ...
// Admin    -> A26-0001, A26-0002 ...
class IDgenerator {
private:
    static int studentCount;
    static int professorCount;
    static int adminCount;
    static int currentYear;

    static int getCurrentYear();
    static void resetIfNewYear();

public:
    static string generateStudentID();
    static string generateProfessorID();
    static string generateAdminID();
};
