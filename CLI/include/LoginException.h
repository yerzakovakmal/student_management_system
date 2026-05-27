#pragma once
#include <stdexcept>
#include <string>
using namespace std;

class LoginException : public runtime_error {
public:
    explicit LoginException(const string& msg) : runtime_error(msg) {}
};

class FileException : public runtime_error {
public:
    explicit FileException(const string& msg) : runtime_error(msg) {}
};

class ValidationException : public runtime_error {
public:
    explicit ValidationException(const string& msg) : runtime_error(msg) {}
};
