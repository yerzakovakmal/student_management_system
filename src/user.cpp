// User.cpp — implementation
#include "../include/user.h"

// Static member definitions ─
int User::totalUsers = 0;

//constructor
User::User(string name, string email, string password, string role, string userId)
    : name(name), email(email), password(password), role(role), userId(userId)
{
    if (name.empty() || email.empty())
        throw invalid_argument("Name and email cannot be empty");
    if (password.length() < 6)
        throw invalid_argument("Password must be at least 6 chars");
    totalUsers++;
}

//Destructor
User::~User() {
    --totalUsers;   // keep the count accurate
}

//Getters
string User::getName() const{
    return name;
}
string User::getEmail() const{
    return email;
}
string User::getUserId() const{
    return userId;
}

string User::getRole() const{
    return role;
}

//Setters
void User::setName(string newName) {
    if (newName.empty())
        throw invalid_argument("Name cannot be empty");
    name = newName;
}

void User::setEmail(string newEmail) {
    if (newEmail.find('@') == string::npos)
        throw invalid_argument("Invalid email format");
    email = newEmail;
}

// Common methods
bool User::login(string inputEmail, string inputPassword) {
    if (email == inputEmail && password == inputPassword) {
        cout << "Login successful. Welcome, " << name << "!" << endl;
        return true;
    }
    throw runtime_error("Invalid email or password");
    return false;
}

void User::logout() {
    cout << name << " has logged out." << endl;
}

void User::updateProfile(string newName, string newEmail) {
    setName(newName);    // reuse setters (with validation)
    setEmail(newEmail);
    cout << "Profile updated successfully." << endl;
}

void User::changePassword(string oldPass, string newPass) {
    if (password != oldPass)
        throw runtime_error("Current password is incorrect");
    if (newPass.length() < 6)
        throw invalid_argument("New password too short");
    if (newPass == oldPass)
        throw invalid_argument("New password must differ from old");
    password = newPass;
    cout << "Password changed successfully.\n";
}

//Static methods
int User::getTotalUsers(){
    return totalUsers;
}
void User::resetUserCount(){ 
    totalUsers = 0;
}

//Operator overloads
bool User::operator==(const User& other) const {
    return userId == other.userId;//unique ID = identity
}

bool User::operator!=(const User& other) const {
    return !(*this == other);//reuse operator==
}

ostream& operator<<(ostream& out, const User& u) {
    out << "[" << u.role << "] "
        << u.name << " <"
        << u.email << ">"
        << " #" << u.userId;
    return out;
}