#pragma once
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class User {
protected:
    string userId;
    string name;
    string email;
    string password;
    static int totalUsers;

public:
    User(string userId, string name, string email, string password);
    virtual ~User();

    string getUserId() const;
    string getName() const;
    string getEmail() const;
    string getPassword() const; // Added for Admin oversight

    bool checkPassword(string input) const;

    // Operator overloading
    bool operator==(const User& other) const;
    friend ostream& operator<<(ostream& out, const User& u);

    // Binary file — shared by all subclasses
    void writeToBinary(ofstream& out) const;
    void readFromBinary(ifstream& in);

    // Pure virtual — each role implements its own panel
    virtual void displayPanel() = 0;
    virtual string getRole() const = 0;

    static int getTotalUsers();
};
