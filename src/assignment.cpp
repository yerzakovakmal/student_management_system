#include "../include/assignment.h"
#include "../include/course.h"
using namespace std;

int Assignment::nextID = 1;

//Constructor ---------------------------------------
Assignment::Assignment(string title, string description, string dueDate, double maxScore, Course* course)
    : assignmentID(nextID++), 
    title(title), 
    description(description), 
    dueDate(dueDate),
    maxScore(maxScore),
    course(course)
{
    if (title.empty())
        throw invalid_argument("Title cannot be empty");
    if (description.empty())
        throw invalid_argument("Description cannot be empty");
    if (dueDate.empty())
        throw invalid_argument("Due date cannot be empty");
    if (maxScore <= 0)
        throw invalid_argument("Max score must be a positive number");
}

//getters ---------------------------------------
int Assignment::getAssignmentID() const{
    return assignmentID;
}

string Assignment::getTitle() const{
    return title;
}

string Assignment::getDescription() const{
    return description;
}

string Assignment::getDueDate() const{
    return dueDate;
}

double Assignment::getMaxScore() const{
    return maxScore;
}

Course* Assignment::getCourse() const{
    return course;
}

int Assignment::getTotalAssignments(){
    return nextID - 1;
}

//setters ---------------------------------------
void Assignment::setTitle(string newTitle){
    if(title.empty())
        throw invalid_argument("Title cannot be empty");
    title = newTitle;
}

void Assignment::setDescription(string desc){
    if(desc.empty()){
        throw invalid_argument("Add description of Assignment");
    }
    description = desc;
}

bool isLeapYear(int year){
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
void Assignment::setDueDate(string date){
    if(date.length() != 10 || date[2] != '-' || date[5] != '-'){
        throw invalid_argument("Due date must be in DD-MM-YYYY format");
    }
    //check if all characters are digit
    for(int i = 0; i < 10; i++){
            if(i == 2 || i == 5)
                continue;
            if(!isdigit(date[i])){
                throw invalid_argument("Date must contain only digits in DD-MM-YYYY format");
            }
    }

    //extract day, month, year
    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    //check month
    if(month < 1 || month > 12){
        throw invalid_argument("Invalid month");
    }

    //days in each month
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    //february adjusting
    if(month == 2 && isLeapYear(year)){
        daysInMonth[1] = 29;
    }

    //check day
    if(day < 1 || day > daysInMonth[month - 1]) {
        throw invalid_argument("Invalid day for given month");
    }

    dueDate = date;
}

void Assignment::setMaxScore(double score){
    if(score <= 0.0){
        throw invalid_argument("Max score must be greater than 0");
    }
    maxScore = score;
}

bool Assignment::isOverdue(string todayDate) const{
    // DD-MM-YYYY lexicographic comparison works correctly
    return dueDate < todayDate;
}

// operator overloading ---------------------------------------
bool Assignment::operator==(const Assignment& other) const{
    return assignmentID == other.assignmentID;
}

bool Assignment::operator!=(const Assignment& other) const{
    return !(*this == other);
}

bool Assignment::operator<(const Assignment& other) const{
    return dueDate < other.dueDate;
}

bool Assignment::operator>(const Assignment& other) const{
    return other < *this; //reuse operator<
}

ostream& operator<<(ostream& out, const Assignment& a){
    out << "[Assignment #" << a.assignmentID << "] " << endl;
    out << a.title << endl;
    out << a.description << endl;
    out << "Due Date: " << a.dueDate << endl;
    out << "Max Score: "<<a.maxScore << endl;
    return out;
}