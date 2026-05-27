#include "../include/course.h"
using namespace std;

// ── helpers for writing / reading a length-prefixed string ────────────────
static void writeStr(ofstream& out, const string& s) {
    int len = (int)s.size();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(s.c_str(), len);
}
static void readStr(ifstream& in, string& s) {
    int len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    s.resize(len);
    in.read(&s[0], len);
}

Course::Course() : capacity(30), credits(3), daysOfWeek("TBD"), timeSlot("TBD") {}

Course::Course(string id, string name, int cred, int cap, string days, string time)
    : courseId(id), courseName(name), credits(cred), capacity(cap), daysOfWeek(days), timeSlot(time) {}

void writeCourseBinary(ofstream& out, const Course& c) {
    writeStr(out, c.courseId);
    writeStr(out, c.courseName);
    writeStr(out, c.professorId);
    out.write(reinterpret_cast<const char*>(&c.capacity), sizeof(c.capacity));
    out.write(reinterpret_cast<const char*>(&c.credits),  sizeof(c.credits));
    writeStr(out, c.daysOfWeek);
    writeStr(out, c.timeSlot);
}

void readCourseBinary(ifstream& in, Course& c) {
    readStr(in, c.courseId);
    readStr(in, c.courseName);
    readStr(in, c.professorId);
    in.read(reinterpret_cast<char*>(&c.capacity), sizeof(c.capacity));
    in.read(reinterpret_cast<char*>(&c.credits),  sizeof(c.credits));
    readStr(in, c.daysOfWeek);
    readStr(in, c.timeSlot);
}
