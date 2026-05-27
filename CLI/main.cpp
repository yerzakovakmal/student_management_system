#include <iostream>
#include <string>
#include "include/User.h"
#include "include/Admin.h"
#include "include/Student.h"
#include "include/Professor.h"
#include "include/LoginException.h"
using namespace std;

// Demo accounts -fixed credentials for the grader to use

Admin     adminAcc("Dr. Admin",      "admin@uni.edu",  "admin123");
Student   studentAcc("Ali Karimov",  "ali@uni.edu",    "ali123");
Professor profAcc("Dr. Nazarov",     "naz@uni.edu",    "naz123",  "Computer Science");

//clear window
static void clearScreen(){
    // ANSI escape clears the window of terminal
    cout << "\033[2J\033[H";
}

// Login: returns a base-class pointer on success, throws on failure
User* doLogin(int role) {
    string email, pass;
    cout << "Email:    "; cin >> email;
    cout << "Password: "; cin >> pass;

    if (role == 1 && studentAcc.getEmail()  == email && studentAcc.checkPassword(pass))
        return &studentAcc;
    if (role == 2 && profAcc.getEmail()     == email && profAcc.checkPassword(pass))
        return &profAcc;
    if (role == 3 && adminAcc.getEmail()    == email && adminAcc.checkPassword(pass))
        return &adminAcc;

    throw LoginException("Invalid email or password.");
}

int main() {
    // Admin loads all persisted data at startup
    adminAcc.loadAllData();

    bool running = true;
    while (running) {
        clearScreen();
        cout << "\n=====================================" << endl;
        cout << "   University Management System      " << endl;
        cout << "=====================================" << endl;
        cout << "  1. Login as Student"    << endl;
        cout << "  2. Login as Professor"  << endl;
        cout << "  3. Login as Admin"      << endl;
        cout << "  0. Exit"                << endl;
        cout << "Choice: ";

        int choice = 0;
        cin >> choice;

        if (choice == 0) {
            cout << "Goodbye." << endl;
            running = false;
            break;
        }

        if (choice < 1 || choice > 3) {
            cout << "Invalid option. Please choose 1, 2, 3, or 0." << endl;
            continue;
        }

        // try-catch block: only login failure is caught here
        try {
            User* current = doLogin(choice);
            cout << "\nWelcome, " << current->getName()
                 << "!  Role: " << current->getRole() << endl;

            // Polymorphic call — each subclass shows its own panel.
            // Panel returns when the user picks 0 (Logout).
            current->displayPanel();

            cout << "\nYou have been logged out." << endl;

        } catch (LoginException& le) {
            cout << "\n[Login Failed] " << le.what() << endl;
            cout << "Returning to main menu..." << endl;
        }
    }

    return 0;
}
