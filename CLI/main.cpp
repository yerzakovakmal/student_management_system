#include <iostream>
#include <string>
#include "include/user.h"
#include "include/admin.h"
#include "include/student.h"
#include "include/professor.h"
#include "include/LoginException.h"
using namespace std;

// Demo accounts fixed credentials
Admin adminAcc("Dr. Admin", "admin", "admin");
Student studentAcc("Ali Karimov", "ali@student.iut.uz", "ali123");
Professor profAcc("Dr. Nazarov", "naz@student.iut.uz", "naz123",  "Computer Science");

//clear window ANSI escape
static void clearScreen(){
    cout << "\033[2J\033[H";
}

// Login: returns a base-class pointer on success, throws on failure
User* doLogin(int role, Admin& admin) {
    string identifier, pass;
    cout << "\n--- Login ---" << endl;
    
    if (role == 1) cout << "Student ID: ";
    else if (role == 2) cout << "Professor ID: ";
    else cout << "Email: ";
    
    cin >> identifier;
    cout << "Password: "; cin >> pass;

    if (role == 1) { // Student
        // Check runtime
        Student* s = admin.findStudent(identifier);
        if (s != NULL && s->checkPassword(pass)){
            return s;
        }
        
        // Check demo account
        if (studentAcc.getUserId() == identifier && studentAcc.checkPassword(pass)){
            return &studentAcc;
        }
    } 
    else if (role == 2) { // Professor
        // Check runtime collection
        Professor* p = admin.findProfessor(identifier);
        if (p != NULL && p->checkPassword(pass)){
            return p;
        }
        // Check demo account
        if (profAcc.getUserId() == identifier && profAcc.checkPassword(pass)) {
            return &profAcc;
        }
    } 
    else if (role == 3) { // Admin
        if (adminAcc.getEmail() == identifier && adminAcc.checkPassword(pass))
            return &adminAcc;
    }

    throw LoginException("Invalid ID/Email or password.");
}

int main() {
    // Admin loads all persisted data at startup
    try {
        adminAcc.loadAllData();
    } catch (exception& e) {
        cout << "[Warning] Could not load data: " << e.what() << endl;
    }

    bool running = true;
    while (running) {
        clearScreen();
        cout << "=====================================" << endl;
        cout << "   University Management System      " << endl;
        cout << "=====================================" << endl;
        cout << "  1. Login as Student" << endl;
        cout << "  2. Login as Professor" << endl;
        cout << "  3. Login as Admin" << endl;
        cout << "  0. Exit" << endl;
        cout << "Choice: ";

        int choice = 0;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        if (choice == 0) {
            cout << "Goodbye." << endl;
            running = false;
            break;
        }

        if (choice < 1 || choice > 3) {
            cout << "Invalid option. Please choose 1, 2, 3, or 0." << endl;
            continue;
        }

        // try-catch block -> only login failure is caught here
        try {
            User* current = doLogin(choice, adminAcc);
            cout << "\nWelcome, " << current->getName() << "!  Role: " << current->getRole() << endl;
            cout << "Press Enter to enter your panel...";
            cin.ignore(); cin.get();

            if (current->getRole() == "Professor") {
                Professor* profPtr = dynamic_cast<Professor*>(current);
                if (profPtr != NULL) {
                    profPtr->displayPanelWithAdmin(adminAcc);
                }
            } else if (current->getRole() == "Student") {
                Student* studPtr = dynamic_cast<Student*>(current);
                if (studPtr != NULL) {
                    studPtr->displayPanelWithAdmin(adminAcc);
                }
            } else {
                // Admin
                current->displayPanel();
            }

            cout << "\nYou have been logged out." << endl;
            cout << "Press Enter to return to main menu...";
            cin.get();

        } catch (LoginException& le) {
            cout << "\n[Login Failed] " << le.what() << endl;
            cout << "Returning to main menu..." << endl;
            cin.ignore(); cin.get();
        }
    }

    return 0;
}
