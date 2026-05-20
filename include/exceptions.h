#pragma once
#include <stdexcept>
#include <string>
using namespace std;


//EnrollmentException
class EnrollmentException : public runtime_error{
    public:
        explicit EnrollmentException(const string& msg)
            : runtime_error(msg){}
};

//CourseFullException
class CourseFullException : public EnrollmentException {
    public:
        explicit CourseFullException(const string& msg)
            : EnrollmentException(msg) {}
};

//GradeOutOfRangeException
class GradeOutOfRangeException : public out_of_range {
    public:
        explicit GradeOutOfRangeException(const string& msg)
            : out_of_range(msg){}
};

//InvalidUserException
class InvalidUserException : public invalid_argument{
    public:
        explicit InvalidUserException(const string& msg)
            : invalid_argument(msg){} 
};

//FileIOException
class FileIOException : public runtime_error {
    public:
        explicit FileIOException(const string& msg)
            : runtime_error(msg){}
};

//PermissionException
class PermissionException : public logic_error {
    public:
        explicit PermissionException(const string& msg)
            : logic_error(msg){}
};