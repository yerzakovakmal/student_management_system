// User.h — abstract base class declaration
#pragma once
#include <string>
#include <iostream>
#include <stdexcept>
using namespace std;

class User {
protected:
    // protected so subclasses can read directly
    string name;
    string email;
    string password;
    string role;
    string userId;

private:
    static int totalUsers; // shared across ALL User instances
public:
    //Constructor & Destructor
    User(string name = "UNKNOWN", string email = "unknown@example.com", string password = "password", string role = "User", string userId = "UNKNOWN");
    virtual ~User(); // virtual — required for polymorphism

    //Getters
    string getName() const;
    string getEmail() const;
    string getUserId() const;

    //Setters (with validation)
    void setName(string name);
    void setEmail(string email);

    //Common methods (all users share these)
    bool login(string email, string password);
    void logout();
    void updateProfile(string newName, string newEmail);
    void changePassword(string oldPass, string newPass);

    //Static methods - keep the counts
    static int getTotalUsers();
    static void resetUserCount();  //for testing

    //Operator overloading
    bool operator==(const User& other) const;
    bool operator!=(const User& other) const;
    friend ostream& operator<<(ostream& out, const User& u);

    //Pure virtual (overridde)
    virtual string getRole() const = 0;
    virtual void displayInfo() const = 0;
};