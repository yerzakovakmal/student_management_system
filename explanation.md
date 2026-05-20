# C++ Concepts Explained — Student Course & Grade Management System

This file explains **why** every decision in our class design was made.
No jargon. If you can read this and understand it, you can explain it to the professor.

---

## Table of Contents

1. [Access Modifiers — private, protected, public](#1-access-modifiers)
2. [Virtual — virtual functions and virtual inheritance](#2-virtual)
3. [Static — static members and static methods](#3-static)
4. [Pure Virtual — abstract classes](#4-pure-virtual--abstract-classes)
5. [Override — overriding virtual functions](#5-override)
6. [Constructors and Destructors](#6-constructors-and-destructors)
7. [Operator Overloading](#7-operator-overloading)
8. [Inheritance — single, hierarchical, multiple](#8-inheritance)
9. [Exception Handling](#9-exception-handling)
10. [Pointers — owning vs non-owning](#10-pointers--owning-vs-non-owning)
11. [File Handling](#11-file-handling)
12. [The ID System — nextOrderNum and generateId()](#12-the-id-system)

---

## 1. Access Modifiers

Access modifiers decide **who is allowed to see or touch a piece of data**.
Think of a house: some rooms are public (living room — anyone can enter),
some are private (your bedroom — only you), some are protected (family room — family only).

### `private`

Only the class itself can read or change this data.
Nobody outside — not even a subclass — can touch it.

```cpp
class User {
private:
    static int totalUsers;  // only User.cpp can change this
    static int nextOrderNum;
};
```

**Why we use it here:**
`totalUsers` and `nextOrderNum` are counters that must be controlled carefully.
If any subclass or outside code could change them freely, the count would become wrong.
So we lock them down to `private` and provide a controlled getter `getTotalUsers()`.

---

### `protected`

The class itself AND its subclasses can access this, but outside code cannot.

```cpp
class User {
protected:
    string name;
    string email;
    string password;
};
```

**Why we use it here:**
`Student`, `Professor`, and `Admin` all need to read `name` and `email` directly —
for example, `Professor::displayInfo()` prints `name`.
If `name` were `private`, every subclass would have to call `getName()` just to print it,
which is unnecessarily verbose.
`protected` gives subclasses direct access while still keeping outside code out.

---

### `public`

Anyone can call these — the class itself, subclasses, and outside code.

```cpp
class User {
public:
    string getName() const;
    bool login(string email, string password);
};
```

**Why we use it here:**
`main.cpp`, Qt screens, and other classes need to call `getName()` and `login()`.
These are the "front door" of the class — the safe, controlled ways to interact with it.

---

### The Rule of Thumb

> Data (fields) → `private` or `protected`
> Methods that the world needs → `public`
> Methods only the class uses internally → `private`

---

## 2. Virtual

The word `virtual` appears in two completely different situations in our code.
It is important not to confuse them.

---

### Virtual Functions — "let the subclass decide what happens"

Imagine you have a list of users and you call `displayInfo()` on each one.
Without `virtual`, C++ decides which `displayInfo()` to call based on the
**type of the pointer** (always `User`), not the **actual object**.
Every call would run `User::displayInfo()` — useless.

With `virtual`, C++ looks at the **actual object** at runtime and calls the right version.

```cpp
// Without virtual:
User* u = new Student(...);
u->displayInfo();  // calls User::displayInfo() — WRONG

// With virtual:
virtual void displayInfo();  // declared in User
u->displayInfo();  // calls Student::displayInfo() — CORRECT
```

**Real example from our project:**

```cpp
vector<User*> allUsers;
allUsers.push_back(new Student(...));
allUsers.push_back(new Professor(...));
allUsers.push_back(new Admin(...));

for (User* u : allUsers) {
    cout << u->getRole() << endl;
    // Without virtual: prints "User" three times
    // With virtual:    prints "Student", "Professor", "Admin"
}
```

This is called **runtime polymorphism** — the correct method is chosen at runtime
based on what the object actually is.

---

### Virtual Destructor — "make sure the right destructor runs"

```cpp
class User {
public:
    virtual ~User();  // virtual destructor
};
```

**Why this matters:**

```cpp
User* u = new Student(...);
delete u;  // which destructor runs?
```

Without `virtual`: only `User::~User()` runs. `Student`'s destructor is skipped.
Any memory `Student` allocated gets leaked.

With `virtual`: C++ first runs `Student::~Student()`, then `User::~User()`.
The correct cleanup order is always followed.

**Rule:** If a class has any `virtual` function, its destructor must also be `virtual`.

---

### Virtual Inheritance — "one shared base, not two copies"

This only matters for `TeachingAssistant`, which inherits from both
`Student` AND `Professor`. Both of those inherit from `User`.
Without `virtual`, `TeachingAssistant` would end up with **two copies of `User`** —
two `name` fields, two `email` fields, total confusion.

```
Without virtual inheritance:        With virtual inheritance:

     User    User                          User
      |        |                          /    \
   Student  Professor                Student  Professor
        \  /                               \  /
    TeachingAssistant                  TeachingAssistant
    (has name TWICE)                   (has name ONCE)
```

```cpp
// The fix — add "virtual" to both parent class declarations:
class Student   : virtual public User { ... };
class Professor : virtual public User { ... };

// Now TeachingAssistant gets exactly one User subobject:
class TeachingAssistant : public Student, public Professor { ... };
```

This is called solving the **diamond problem**.

---

## 3. Static

`static` means the member **belongs to the class itself**, not to any individual object.
All objects share one copy of a static member.

### Static Fields

```cpp
class Student : virtual public User {
private:
    static int nextOrderNum;   // shared by ALL Student objects
    static int enrollmentYear; // shared by ALL Student objects
};
```

**Analogy:**
Imagine a factory that builds cars. Each car has its own color, model, speed.
But the factory has one counter on the wall: "Cars built so far: 4271".
That counter belongs to the factory (the class), not to any individual car (object).

In our system: every time a new `Student` is created, `nextOrderNum` increments by 1.
It does not matter which student object you look at — the counter is on the "factory wall".

```cpp
Student s1("Ali", ...);    // nextOrderNum becomes 1 → ID = "U2510001"
Student s2("Zara", ...);   // nextOrderNum becomes 2 → ID = "U2510002"
Student s3("Omar", ...);   // nextOrderNum becomes 3 → ID = "U2510003"
```

---

### Static Methods

```cpp
class User {
public:
    static int getTotalUsers();   // can be called without any object
};

// Called like this:
cout << User::getTotalUsers();    // no object needed — use the class name
```

A static method cannot access `this` or any non-static fields.
It only works with static data.

**Why `generateId()` is static:**
When the constructor runs, the object does not exist yet.
You cannot call a regular method on something that hasn't been built.
But you need the ID before finishing construction.
`static` methods can be called before (or during) object creation.

```cpp
Student::Student(string name, string email, string password)
    : User(name, email, password, "Student", Student::generateId())
    //                                       ^^ called before the object exists
{}
```

---

### Static Member Definition in `.cpp`

A common mistake: forgetting to define static members in the `.cpp` file.

```cpp
// Student.hpp — declaration only:
static int nextOrderNum;

// Student.cpp — REQUIRED definition:
int Student::nextOrderNum = 1;
int Student::enrollmentYear = 25;
```

Without the `.cpp` definition, the linker will throw:
`undefined reference to Student::nextOrderNum` — even though you declared it.

---

## 4. Pure Virtual — Abstract Classes

A **pure virtual function** is a function with `= 0` at the end.
It means: "I am declaring this function exists, but I refuse to implement it.
Every subclass MUST provide their own implementation."

```cpp
class User {
public:
    virtual string getRole()     const = 0;  // pure virtual
    virtual void   displayInfo() const = 0;  // pure virtual
};
```

A class with at least one pure virtual function is called an **abstract class**.
You cannot create an object of an abstract class directly:

```cpp
User u("Ali", ...);  // COMPILE ERROR — User is abstract
```

**Why we make `User` abstract:**
A `User` by itself makes no sense in our system. There is no such thing as
"just a user" — everyone is either a Student, Professor, Admin, or TA.
Making `User` abstract enforces this. The compiler will refuse to let anyone
accidentally create a plain `User` object.

**Every subclass is forced to answer the question:**
"What role are you?" and "How do you display yourself?"

```cpp
// Student must implement:
string getRole() const override { return "Student"; }
void displayInfo() const override { cout << "Student: " << name; }

// Professor must implement:
string getRole() const override { return "Professor"; }
void displayInfo() const override { cout << "Professor: " << name; }
```

If a subclass forgets to implement a pure virtual function, it also becomes
abstract and cannot be instantiated — the compiler tells you immediately.

---

## 5. Override

`override` is a safety keyword you write after a method signature in a subclass.
It tells the compiler: "I intend this to replace a virtual method from the parent.
Please check that a matching virtual method actually exists."

```cpp
class Student : virtual public User {
public:
    string getRole() const override;     // safe
    void   displayInfo() const override; // safe
};
```

**Why bother writing `override`?**

Without it, a typo becomes a silent bug:

```cpp
// User declares:
virtual string getRole() const;

// Student accidentally writes:
string getRole();  // missing "const" — this is a NEW function, not an override!
```

Without `override`, the compiler says nothing. The old `User::getRole()` is still
called through base pointers. Hours of debugging follow.

With `override`, the compiler immediately says:
`error: 'getRole' marked override but does not override any member function`
You fix the typo in seconds.

**Rule:** Always write `override` on every method that is meant to replace a virtual method.

---

## 6. Constructors and Destructors

### Constructor — "setting up the object"

The constructor runs automatically when an object is created.
Its job is to give every field a valid starting value.

```cpp
User::User(string name, string email, string password,
           string role, string userId)
    : name(name), email(email), password(password),
      role(role), userId(userId)
{
    if (name.empty() || email.empty())
        throw InvalidUserException("Name and email cannot be empty");
    if (password.length() < 6)
        throw InvalidUserException("Password must be at least 6 characters");
    ++totalUsers;
}
```

The `: name(name), email(email)...` part is called the **initializer list**.
It is faster than assigning inside the body because it initializes directly
instead of first default-constructing then assigning.

**Validation in the constructor** means it is impossible to create an invalid object.
If the name is empty, the object simply does not get created — an exception is thrown instead.

---

### Destructor — "cleaning up when the object dies"

```cpp
User::~User() {
    --totalUsers;  // keep the count accurate
}
```

The destructor runs automatically when an object goes out of scope or is `delete`d.
Our destructor decrements `totalUsers` so the count always reflects
how many `User` objects currently exist in memory.

---

### The TeachingAssistant Constructor — the trickiest one

With virtual inheritance, the rule is:
**the most derived class must call the virtual base constructor directly.**

```cpp
TeachingAssistant::TeachingAssistant(
    string name, string email, string password,
    string studentId, string empId, string dept, double stipend)
    : User(name, email, password, "TeachingAssistant", studentId), // ← MUST be first
      Student(name, email, password),   // Student's call to User is IGNORED
      Professor(name, email, password, empId, dept), // Professor's call to User is IGNORED
      assignedCourse(nullptr),
      hoursWorked(0),
      stipend(stipend)
{}
```

The `Student(...)` and `Professor(...)` constructors both try to call `User(...)` themselves.
The compiler silently ignores those calls and uses only the one in `TeachingAssistant`.
This guarantees exactly one `User` subobject is constructed.

If you forget the `User(...)` call in `TeachingAssistant`'s initializer list,
the compiler will call `User`'s **default constructor** (one with no arguments).
Since we did not define a default constructor, you get a compile error.

---

## 7. Operator Overloading

Operator overloading lets you define what `==`, `!=`, `<<`, `>` etc. mean
for your custom classes.

### `operator==` — comparing two objects

```cpp
bool User::operator==(const User& other) const {
    return userId == other.userId;
}
```

**Why compare by `userId` and not by `name`?**
Two students could have the same name (Ali Karimov is very common).
The `userId` is guaranteed unique — it was auto-generated by the counter.
Comparing by `userId` means: "are these the exact same person in the system?"

Usage:
```cpp
Student s1("Ali", ...);
Student s2("Ali", ...);  // different person, same name

if (s1 == s2)  // calls operator==
    cout << "same person";
else
    cout << "different people";  // this prints — different userId
```

---

### `operator!=` — reusing `operator==`

```cpp
bool User::operator!=(const User& other) const {
    return !(*this == other);  // just flip operator==
}
```

Never implement the same logic twice. `!=` is always the opposite of `==`,
so we just call `==` and negate the result.

---

### `operator<<` — printing an object with cout

```cpp
ostream& operator<<(ostream& os, const User& u) {
    os << "[" << u.role << "] " << u.name << " <" << u.email << "> #" << u.userId;
    return os;
}
```

This is a **friend function** — it is not a method of `User` but it can access
`User`'s private members because of the `friend` declaration in the header.

**Why return `ostream&`?**
So you can chain multiple `<<` together:
```cpp
cout << student1 << " and " << student2 << endl;
// This works because each << returns the same cout back
```

If it returned `void`, only one `<<` would work per line.

---

### `operator<` on Assignment — sorting by due date

```cpp
bool Assignment::operator<(const Assignment& other) const {
    return dueDate < other.dueDate;
}
```

Dates stored as `"YYYY-MM-DD"` sort correctly with plain string comparison
because the format is most-significant part first.
`"2025-03-01" < "2025-04-15"` is true — March comes before April.

This lets you sort a vector of assignments by due date with zero extra code:
```cpp
sort(assignments.begin(), assignments.end()); // uses operator< automatically
```

---

### `operator>` on Grade — who scored higher?

```cpp
bool Grade::operator>(const Grade& other) const {
    return score > other.score;
}
```

Usage:
```cpp
if (gradeA > gradeB)
    cout << "Student A scored higher";
```

---

## 8. Inheritance

### Single Inheritance

One parent, one child. The child gets everything the parent has.

```cpp
class Student : virtual public User { ... };
```

`Student` automatically has `name`, `email`, `login()`, `logout()` etc.
from `User`, without rewriting them.

---

### Hierarchical Inheritance

One parent, multiple children.

```
        User
       / | \
  Student Prof Admin
```

All three share the same `User` base. Any code that works with a `User*`
automatically works with a `Student*`, `Professor*`, or `Admin*`.

```cpp
// This function works for ALL user types:
void printUserInfo(User* u) {
    cout << u->getRole() << ": " << u->getName() << endl;
}

printUserInfo(new Student(...));   // prints "Student: Ali"
printUserInfo(new Professor(...)); // prints "Professor: Dr. Smith"
printUserInfo(new Admin(...));     // prints "Admin: Sara"
```

---

### Multiple Inheritance

One child, multiple parents. Only `TeachingAssistant` uses this.

```cpp
class TeachingAssistant : public Student, public Professor { ... };
```

A TA object has everything from both `Student` (GPA, enrolled courses, grades)
and `Professor` (department, taught courses, grading ability).

**The risk:** both parents also inherit from `User`, creating the diamond problem.
**The fix:** `virtual` inheritance on both parent classes (explained in section 2).

---

## 9. Exception Handling

Exceptions are how we handle errors without crashing or returning confusing codes.

### Throwing an exception

```cpp
void User::setEmail(string newEmail) {
    if (newEmail.find('@') == string::npos)
        throw InvalidUserException("Invalid email format");
    email = newEmail;
}
```

`throw` immediately stops the current function and jumps to the nearest
matching `catch` block up the call stack.

---

### Catching an exception

```cpp
try {
    student.enrollCourse(course);
} catch (CourseFullException& e) {
    cout << "Cannot enroll: " << e.what() << endl;
} catch (EnrollmentException& e) {
    cout << "Enrollment error: " << e.what() << endl;
}
```

**Why catch the specific type first?**
`CourseFullException` inherits from `EnrollmentException`.
If you put `EnrollmentException` first, it catches everything — including
`CourseFullException` — and the specific handler never runs.
Always catch the most specific type first, most general last.

---

### Our custom exception hierarchy

```
runtime_error
└── EnrollmentException
    └── CourseFullException

out_of_range
└── GradeOutOfRangeException

invalid_argument
└── InvalidUserException

runtime_error
└── FileIOException
```

**Why inherit from standard exceptions instead of writing from scratch?**
Code that catches `runtime_error` will also catch `EnrollmentException`
because of inheritance. Our custom exceptions plug into the standard
exception system — they do not replace it.

**Why not just use `runtime_error` everywhere?**
Because `catch (CourseFullException&)` is more precise than `catch (runtime_error&)`.
Specific types let you handle different problems differently.
A full course and a wrong password are both errors, but they need different responses.

---

## 10. Pointers — Owning vs Non-Owning

This is one of the most common sources of bugs in C++.
A pointer is either **owning** (responsible for deleting) or **non-owning** (just borrowing).

### Owning pointer

The object was created with `new` by this class, so this class must `delete` it.

```cpp
// Admin creates a Student — Admin owns it
Student* Admin::createStudent(string name, string email, string password) {
    return new Student(name, email, password);  // caller must delete this
}
```

---

### Non-owning pointer

This class received a pointer from somewhere else. It must NOT delete it.

```cpp
class Course {
private:
    vector<Student*> enrolledStudents;  // non-owning — students exist elsewhere
    Professor* professor;               // non-owning — professor exists elsewhere
};

Course::~Course() {
    // Do NOT delete professor or any student pointer
    // They were not created here — someone else will clean them up
}
```

**What happens if you delete a non-owning pointer?**
The object gets deleted twice — once by the real owner, once by the non-owner.
This is called a **double-free** and causes undefined behavior (usually a crash).

---

### Checking for nullptr

Any non-owning pointer might be `nullptr` if nothing has been assigned yet.
Always check before using:

```cpp
void TeachingAssistant::gradeAssignment(Student& s, double score) {
    if (!assignedCourse)
        throw runtime_error("TA has no assigned course yet");
    // safe to use assignedCourse below
    s.receiveGrade(*assignedCourse, score);
}
```

---

## 11. File Handling

File handling lets the program save data between runs.
Without it, everything is lost when the program closes.

### Saving to a file

```cpp
void Course::saveToFile(string filename) {
    ofstream file(filename);
    if (!file.is_open())
        throw FileIOException("Could not open file: " + filename);

    file << courseId   << "\n"
         << courseName << "\n"
         << credits    << "\n"
         << capacity   << "\n";

    file.close();
}
```

`ofstream` = output file stream = write to a file.
We write one field per line so it is easy to read back in order.

---

### Loading from a file

```cpp
Course Course::loadFromFile(string filename) {
    ifstream file(filename);
    if (!file.is_open())
        throw FileIOException("Could not open file: " + filename);

    string courseId, courseName;
    int credits, capacity;

    getline(file, courseId);
    getline(file, courseName);
    file >> credits >> capacity;

    file.close();
    return Course(courseId, courseName, credits, capacity);
}
```

`ifstream` = input file stream = read from a file.
`getline` reads one full line including spaces.
`file >> variable` reads one word or number (stops at whitespace).

**Why `loadFromFile` is `static`:**
You call it without an existing `Course` object — it creates one from the file.
Static methods can be called using just the class name: `Course::loadFromFile("cs101.txt")`.

---

## 12. The ID System

### The problem

We need every user to have a unique, readable ID like `U2510001`.
It must be assigned automatically — no one should have to type it manually.

### The solution: static counter + formatter

Each class keeps its own static counter:

```cpp
// Student.cpp
int Student::nextOrderNum   = 1;
int Student::enrollmentYear = 25;  // last two digits of 2025
```

A static method formats the counter into the ID string:

```cpp
string Student::generateId() {
    ostringstream oss;
    oss << "U"
        << setw(2) << setfill('0') << enrollmentYear  // "25"
        << setw(4) << setfill('0') << nextOrderNum++;  // "0001", "0002", ...
    return oss.str();  // "U2510001"
}
```

`setw(4)` means "use at least 4 characters wide".
`setfill('0')` means "pad with zeros if shorter".
So order number 1 becomes `"0001"`, not just `"1"`.

The constructor calls `generateId()` and passes the result up to `User`:

```cpp
Student::Student(string name, string email, string password)
    : User(name, email, password, "Student", Student::generateId())
{}
```

### Why each class has its own counter

`Student` counter → U IDs
`Professor` counter → P IDs
`Admin` counter → A IDs

They are independent. Creating 3 students and 2 professors gives:
- `U2510001`, `U2510002`, `U2510003`
- `P2510001`, `P2510002`

No collisions. Each role's numbering starts fresh from 1.

### Changing the year

When a new academic year starts, one line changes:

```cpp
Student::enrollmentYear = 26;  // now IDs start with U26XXXX
```

The counter resets too so the new cohort starts from `U2610001`.

---

*End of explanation file.*
*If anything here is still unclear, ask — that is what this file is for.*
