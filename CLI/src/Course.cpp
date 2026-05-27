#include "../include/Course.h"
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

Course::Course() : capacity(30), credits(3) {}

Course::Course(string id, string name, int cred, int cap)
    : courseId(id), courseName(name), credits(cred), capacity(cap) {}

void writeCourseBinary(ofstream& out, const Course& c) {
    writeStr(out, c.courseId);
    writeStr(out, c.courseName);
    writeStr(out, c.professorId);
    out.write(reinterpret_cast<const char*>(&c.capacity), sizeof(c.capacity));
    out.write(reinterpret_cast<const char*>(&c.credits),  sizeof(c.credits));
}

void readCourseBinary(ifstream& in, Course& c) {
    readStr(in, c.courseId);
    readStr(in, c.courseName);
    readStr(in, c.professorId);
    in.read(reinterpret_cast<char*>(&c.capacity), sizeof(c.capacity));
    in.read(reinterpret_cast<char*>(&c.credits),  sizeof(c.credits));
}
