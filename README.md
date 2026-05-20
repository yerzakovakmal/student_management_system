# Student Course & Grade Management System
*The main goal of this project is to develop a GUI-based Student Course & Grade Management System in C++ that helps organize academic information more efficiently for students and instructors. The system is designed to simplify course management, grade recording, and student data handling through a user-friendly interface.*


# Problem Statement
- Managing student, course, and grade information manually can be slow and confusing
- Existing systems may require too many steps for simple academic tasks
- Professors may face difficulties while recording and updating grades
- Students may not have a clear and organized way to view their academic progress

# Target Users
- Students
- Professors
- Academic Admins

# Expected Outcomes
- Faster organization of student and course data
- Easier grade recording and updating
- Better usability through a clean GUI
- Reduced manual workload for both students and professors
- More efficient academic data management overall

## File Structure

```
project/
├── src/
│   ├── User.hpp / User.cpp             ← Akbarali
│   ├── Student.hpp / Student.cpp       ← Malika (header + methods)
│   ├── Professor.hpp / Professor.cpp   ← Akmal
│   ├── TeachingAssistant.hpp / .cpp  
│   ├── Admin.hpp / Admin.cpp           ← Akmal
│   ├── Course.hpp / Course.cpp         ← Akbarali
│   ├── Assignment.hpp / Assignment.cpp ← Rushana (header + methods)
│   ├── Grade.hpp / Grade.cpp           ← Rushana
│   ├── Exceptions.hpp                  ← Akmal
│   └── main.cpp                        ← Akmal + Akbarali
├── qt/
│   ├── MainWindow.hpp / .cpp / .ui     ← Xondamir/Nodir
│   ├── LoginDialog.hpp / .cpp / .ui    ← Xondamir/Nodir
│   ├── AdminPanel.hpp / .cpp / .ui     ← Xondamir/Nodir
│   ├── CourseManager.hpp / .cpp / .ui  ← Xondamir/Nodir
│   ├── GradeEntry.hpp / .cpp / .ui     ← Xondamir/Nodir
│   ├── StudentDashboard.hpp / .cpp / .ui ← Malika
│   └── AssignmentViewer.hpp / .cpp / .ui ← Rushana
├── data/
│   └── (saved .txt files from file I/O)
└── README.md
```

---
*(work in progress...)*

# Team Task Board

---

## Team Structure

| Member | Role | Focus |
|---|---|---|
| Akmal | C++ Lead | User, Student, Professor, TA, Admin — core hierarchy |
| Akbarali | C++ Dev | Course, Assignment, Grade, Exceptions, File I/O |
| Xondamir | Qt Lead | Main window, navigation, Admin panel, Course screens |
| Nodir | Qt Dev | Login screen, Grade entry, Student dashboard |
| Malika | C++ + Presenter | Student methods + presentation slides |
| Rushana | C++ + Presenter | Grade & Assignment methods + presentation slides |

---

## ID Format Convention

Every user gets a unique, human-readable ID generated automatically at construction.

| Role | Prefix | Example | Format |
|---|---|---|---|
| Student | `U` | `U2510001` | U + 2-digit year + 4-digit order |
| Professor | `P` | `P2510001` | P + 2-digit year + 4-digit order |
| Admin | `A` | `A2510001` | A + 2-digit year + 4-digit order |

**How it works in C++:**
```cpp
// Each subclass has its own static counter — defined in .cpp
static int nextOrderNum;    // starts at 1, increments per class
static int enrollmentYear;  // e.g. 25 for 2025

static string generateId(const string& prefix) {
    ostringstream oss;
    oss << prefix
        << setw(2) << setfill('0') << enrollmentYear   // "25"
        << setw(4) << setfill('0') << nextOrderNum++;  // "0001"
    return oss.str();  // e.g. "U2510001"
}
```

---

## C++ Tasks — Akmal, Akbarali, Malika, Rushana

> **Rule:** Every class gets its own `.hpp` and `.cpp` file.
> No implementation inside `.hpp` except single-line getters.
> Always compile and test individually before marking done.

---

### TASK C-01 — `User` abstract base class
**Assigned to:** Akbarali
**Files:** `User.hpp`, `User.cpp`

**What to include:**
- `protected` fields: `name`, `email`, `password`, `role`, `userId` (string)
- `private static` fields: `totalUsers` (int)
- Constructor takes `(name, email, password, role, userId)` — subclass passes the formatted ID in
- Validation: empty name/email throws `InvalidUserException`, password under 6 chars throws `InvalidUserException`
- Virtual destructor — decrements `totalUsers`
- Getters: `getName()`, `getEmail()`, `getUserId()`
- Setters with validation: `setName()`, `setEmail()` — checks for `@` character
- Methods: `login()`, `logout()`, `updateProfile()`, `changePassword()`
- Static: `getTotalUsers()`
- `operator==` — compares by `userId`, `operator!=`, `operator<<`
- **Pure virtual:** `getRole() const = 0`, `displayInfo() const = 0`

---

### TASK C-02 — `Professor` class
**Assigned to:** Akmal
**Files:** `Professor.hpp`, `Professor.cpp`

**What to include:**
- Inherits: `virtual public User`
- Static fields: `nextOrderNum`, `enrollmentYear`
- Static method: `generateId()` — returns e.g. `"P2510001"`
- Fields: `department` (string), `taughtCourses` (vector\<Course*\>) — non-owning
- Constructor: calls `generateId()`, passes result to `User(...)`
- Methods:
  - `createCourse(Course*)` — adds to `taughtCourses`
  - `assignGrade(Student&, Course&, double)` — throws `GradeOutOfRangeException` if score not 0–100
  - `viewRoster(Course&)` — returns `vector<Student*>`, does NOT print (Qt will display it)
  - `removeStudentFromCourse(Student&, Course&)`
  - `postMaterial(string, Course&)`
  - `createAssignment(Course&, string title, double maxScore)`
  - `generateReport(Course&) const` — returns a `string` built with `ostringstream`
  - `assignTA(TeachingAssistant&, Course&)`
- `operator==` — compares by `userId`, `operator<<`
- Override: `getRole()` returns `"Professor"`, `displayInfo()`

---

### TASK C-03 — `TeachingAssistant` class (might not be included)
**Assigned to:** Akmal
**Files:** `TeachingAssistant.hpp`, `TeachingAssistant.cpp`

**What to include:**
- Inherits: `public Student, public Professor` — multiple inheritance
- TA keeps the student ID format (`U25XXXX`) — no separate prefix needed
- Fields: `assignedCourse` (Course*, non-owning), `hoursWorked` (int), `stipend` (double)
- Constructor initializer list **must** call `User(...)` explicitly first, then `Student(...)`, then `Professor(...)`
- Methods: `gradeAssignment()`, `postAnnouncement()`, `holdOfficeHours()`, `answerQuestion()`, `assistProfessor()`, `viewCourseRoster()`, `logHours()`, `setAssignedCourse()`
- Every method using `assignedCourse` throws `runtime_error` if it is `nullptr`
- `operator==` — compares by `getUserId()`, `operator<<`
- **Must** override both `getRole()` (returns `"TeachingAssistant"`) and `displayInfo()`

---

### TASK C-04 — `Admin` class
**Assigned to:** Akmal
**Files:** `Admin.hpp`, `Admin.cpp`

**What to include:**
- Inherits: `virtual public User`
- Static fields: `nextOrderNum`, `enrollmentYear`
- Static method: `generateId()` — returns e.g. `"A2510001"`
- Fields: `adminDepartment` (string), `permissions` (vector\<string\>)
- Methods:
  - `createStudent(name, email, password, year)` — constructs and returns `Student*`
  - `createProfessor(name, email, password, dept, year)` — constructs and returns `Professor*`
  - `deleteUser(User*)` — prints confirmation (caller handles actual deletion)
  - `viewAllStudents(vector<Student*>&)` — returns or prints the full list
  - `viewAllProfessors(vector<Professor*>&)`
  - `assignProfessorToCourse(Professor&, Course&)`
  - `generateSystemReport()` — returns string: total users, total courses
  - `hasPermission(string) const`, `addPermission(string)`, `revokePermission(string)`
- `operator==` — compares by `userId`, `operator<<`
- Override: `getRole()` returns `"Admin"`, `displayInfo()`

---

### TASK C-05 — `Student` class
**Assigned to:** Malika
**Files:** `Student.hpp`, `Student.cpp`

In `Student.hpp`:**
- Inherits: `virtual public User`
- Static fields: `nextOrderNum`, `enrollmentYear`
- Static method: `generateId()` — returns e.g. `"U2510001"`
- Fields: `gpa` (double), `enrolledCourses` (vector\<Course*\>), `grades` (vector\<Grade\>)
- Constructor: calls `generateId()`, passes to `User(...)`
- `operator==`, `operator<<`
- Override declarations: `getRole()`, `displayInfo()`

Methods in `Student.cpp`:**
- `enrollCourse(Course& c)` — adds course pointer to `enrolledCourses`, throws `CourseFullException` if `c.isFull()`
- `dropCourse(Course& c)` — removes course from `enrolledCourses`, throws `EnrollmentException` if not enrolled
- `viewGrades()` — loops over `grades` vector and prints each one using `operator<<`
- `calculateGPA()` — loops over `grades`, averages all scores, updates and returns `gpa`
- `submitAssignment(Assignment& a)` — prints confirmation that assignment was submitted
- `viewTranscript()` — prints all courses and their corresponding grade in a formatted block
- `receiveGrade(Course& c, double score)` — called by Professor/TA, constructs a `Grade` and pushes to `grades`
- `getRole()` — returns `"Student"`
- `displayInfo()` — prints all student fields neatly

**How to think about it:**
> You are filling in the function bodies — Akmal gives you the `.hpp` with the declarations.
> Your job is `.cpp` implementations only. Start with `getRole()` and `displayInfo()` — they are one-liners.
> Then do `calculateGPA()` — it is a simple loop and average. Leave `enrollCourse()` and `dropCourse()` for last as they use exceptions.

---

### TASK C-06 — `Course` class
**Assigned to:** Akbarali
**Files:** `Course.hpp`, `Course.cpp`

**What to include:**
- Fields: `courseId` (string), `courseName` (string), `credits` (int), `capacity` (int), `enrolledStudents` (vector\<Student*\>), `assignments` (vector\<Assignment\>), `materials` (vector\<string\>), `announcements` (vector\<string\>), `professor` (Professor*, non-owning)
- `static int courseCount`
- Constructor: validates `capacity > 0`, increments `courseCount`
- Methods: `addStudent()` — throws `CourseFullException` if full, `removeStudent()`, `getEnrolledStudents()`, `addMaterial()`, `addAssignment()`, `addAnnouncement()`, `getCourseName()`, `getCourseId()`, `isFull() const`
- `operator==` — compares by `courseId`, `operator<<`
- `saveToFile(string filename)` — writes each field line by line
- `static Course loadFromFile(string filename)` — reads back, throws `FileIOException` if file missing

---

### TASK C-07 — `Assignment` class
**Assigned to:** Rushana
**Files:** `Assignment.hpp`, `Assignment.cpp`

Header + skeleton in Header file:**
- Fields: `assignmentId` (int, auto-increment static counter), `title` (string), `dueDate` (string `"YYYY-MM-DD"`), `maxScore` (double), `course` (Course*, non-owning)
- Constructor: throws `invalid_argument` if `maxScore <= 0` or `title` empty
- Declaration of all methods and operators

Methods in `Assignment.cpp`:**
- `getTitle()` — returns `title`
- `getDueDate()` — returns `dueDate`
- `getMaxScore()` — returns `maxScore`
- `getAssignmentId()` — returns `assignmentId`
- `operator<(const Assignment& other) const` — compares `dueDate` strings (lexicographic works for `YYYY-MM-DD` format)
- `operator==(const Assignment& other) const` — compares by `assignmentId`
- `operator<<` — prints title, due date, max score neatly

**How to think about it:**
> Getters are one-liners: `return title;`. The interesting part is `operator<` —
> you are comparing two date strings like `"2025-03-01"` and `"2025-04-15"`.
> Because the format is year-month-day (biggest unit first), plain string comparison
> gives the correct date order automatically. Just write `return dueDate < other.dueDate;`.

---

### TASK C-08 — `Grade` class
**Assigned to:** Akbarali
**Files:** `Grade.hpp`, `Grade.cpp`

**What to include:**
- Fields: `student` (Student*, non-owning), `assignment` (Assignment*, non-owning), `score` (double), `letterGrade` (char), `feedback` (string)
- Constructor: receives student pointer, assignment pointer, and score — calls `calculateLetterGrade()` immediately
- Private method `calculateLetterGrade()`:
  - score >= 90 → `'A'`
  - score >= 80 → `'B'`
  - score >= 70 → `'C'`
  - score >= 60 → `'D'`
  - below 60   → `'F'`
- `setScore(double)` — throws `GradeOutOfRangeException` if score < 0 or > assignment's maxScore, then calls `calculateLetterGrade()` again
- `getScore()`, `getLetterGrade()`, `getFeedback()`, `setFeedback(string)`
- `operator>(const Grade& other) const` — compares scores
- `operator==(const Grade& other) const` — compares by student userId AND assignment id
- `operator<<` — prints student name, score, letter grade, feedback
- `saveToFile(string filename)` — writes fields line by line
- `loadFromFile(string filename)` — reads back

**How to think about it:**
> The key rule: `calculateLetterGrade()` must be called in TWO places —
> inside the constructor (when the grade is first created) AND inside `setScore()`
> (when the score is changed later). If you forget the second call, the letter grade
> will show the old value even after the score changes. That is a bug.
> `operator>` is just `return score > other.score;` — one line.

---

### TASK C-09 — Exception classes
**Assigned to:** Akbarali
**Files:** `Exceptions.hpp` (header only — no `.cpp` needed)

**What to include:**
```cpp
class EnrollmentException     : public runtime_error { ... };
class CourseFullException      : public EnrollmentException { ... };
class GradeOutOfRangeException : public out_of_range { ... };
class InvalidUserException     : public invalid_argument { ... };
class FileIOException          : public runtime_error { ... };
```
Each is just a constructor forwarding a message to the parent:
```cpp
explicit CourseFullException(const string& msg)
    : EnrollmentException(msg) {}
```

---

### TASK C-10 — `main.cpp` integration demo
**Assigned to:** Akmal + Akbarali (together, after all classes compile)
**Files:** `main.cpp`

**What to include:**
- Create 1 Admin — use Admin to create 1 Professor and 2 Students
- Print all generated IDs: `U2510001`, `P2510001`, `A2510001`
- Professor creates a Course, Admin assigns Professor to it
- Professor assigns TA to course
- Professor assigns grades to both Students
- TA grades an assignment
- Demonstrate polymorphism: store all users in `vector<User*>`, loop and call `getRole()` on each
- Call `User::getTotalUsers()` — must show correct count
- Save a Course to file, reload it, print name to confirm file I/O
- Wrap all risky calls in try-catch using custom exceptions

---

## Qt Tasks — Xondamir, Nodir, Malika, Rushana

> Start Qt after C-01 through C-05 compile cleanly.
> Qt is a wrapper — it calls your C++ methods, never reimplements the logic.
> Use Qt Style Sheets (QSS) to style widgets — syntax is nearly identical to CSS.

---

### TASK Q-01 — Project setup & main window skeleton
**Assigned to:** Xondamir/Nodir

- Set up Qt Creator project, link all C++ `.cpp` files
- Create `MainWindow` with sidebar navigation: Dashboard, Users, Courses, Grades
- Role-based view switching: Admin panel / Professor view / Student view shown depending on login role

---

### TASK Q-02 — Login screen
**Assigned to:** Xondamir/Nodir

- Email + password input fields, login button
- On submit: calls `user.login()`, catches `runtime_error`, shows `QMessageBox` with error text on failure
- On success: calls `getRole()` and switches to the correct main view

---

### TASK Q-03 — Admin panel (user management)
**Assigned to:** Xondamir/Nodir

- `QTableWidget` showing all students and professors with their IDs
- "Create Student" and "Create Professor" buttons — open a `QDialog` form
- Form calls `admin.createStudent(...)` or `admin.createProfessor(...)` on submit
- Generated ID appears in the table immediately after creation

---

### TASK Q-04 — Course management (Professor view)
**Assigned to:** Xondamir/Nodir

- List of courses the professor teaches
- "Create course" button → `QDialog` form → calls `professor.createCourse(...)`
- "View roster" button → calls `professor.viewRoster(course)` and displays the returned vector in a `QTableWidget`
- "Remove student" button → calls `professor.removeStudentFromCourse(...)`

---

### TASK Q-05 — Grade entry screen
**Assigned to:** Xondamir/Nodir

- Student dropdown (`QComboBox`), score input (`QDoubleSpinBox` capped at 100)
- Submit calls `professor.assignGrade(...)`, wraps in try-catch
- Shows `GradeOutOfRangeException` message in `QMessageBox` if score is invalid

---

### TASK Q-06 — Student dashboard
**Assigned to:** Malika

- `QTableWidget` displaying enrolled courses and grade for each
- GPA label — calls `student.calculateGPA()` and displays result
- "View transcript" button — calls `student.viewTranscript()`, shows result in a `QTextEdit` dialog
- Refresh button to reload grades after a new one is assigned

**How to think about it (Malika):**
> You already implemented `calculateGPA()` and `viewTranscript()` in C++ Task C-05.
> In Qt your job is to call those same methods and display what they return.
> A `QLabel` shows text: `gpaLabel->setText(QString::number(student.calculateGPA(), 'f', 2));`
> A `QTableWidget` has rows and columns — one row per course, columns for course name and grade.

---

### TASK Q-07 — Assignment & Grade viewer
**Assigned to:** Rushana

- `QTableWidget` showing all assignments for a course: title, due date, max score
- Sorted by due date (call `std::sort` on the vector using `operator<` you wrote)
- Click an assignment → show a detail panel with all student grades for that assignment
- Each row shows student name, score, letter grade

**How to think about it:**
> You implemented `operator<` on `Assignment` and `operator<<` on `Grade` in C++ Tasks C-07 and C-08.
> Here you use the sorted vector to populate a `QTableWidget`.
> Each cell is set with `table->setItem(row, col, new QTableWidgetItem("text"))`.
> The letter grade column can be colored: A = green background, F = red background using `setBackground()`.

---

## Presentation Tasks — Malika & Rushana

> These run in parallel with coding. Start early — do not leave slides for the last day.

### TASK P-01 — Slide deck structure
**Assigned to:** Malika & Rushana (divide slides between you)

**Slide outline:**
1. Project overview — what the system does, who the users are
2. Class hierarchy diagram — export the UML we built
3. OOP concepts used — one slide each for: inheritance, polymorphism, operator overloading, static members, exceptions
4. ID system explanation — how `U2510001` is generated
5. Qt UI walkthrough — screenshots of each screen
6. File I/O demo — show saving and loading a course
7. Team contributions — who built what
8. Live demo flow — step by step what to show during the presentation

### TASK P-02 — Code snippet slides
**Assigned to:** Malika

- Pick 3–4 short, clear code examples from the project
- Good candidates: the virtual inheritance constructor, `calculateGPA()`, `operator<<`, a try-catch block
- Keep each snippet under 15 lines — the audience cannot read walls of code on a slide

### TASK P-03 — Individual prep
**Assigned to:** everyone

- Each member should be ready to explain their own files line by line
- Malika: `enrollCourse()`, `calculateGPA()`, `viewTranscript()`, `receiveGrade()` from Student + Q-06
- Rushana: `operator<` on Assignment, `calculateLetterGrade()`, `setScore()` from Grade + Q-07
- Akmal: virtual inheritance constructor in TeachingAssistant, `generateId()`, pure virtual in User
- Akbarali: `saveToFile()` / `loadFromFile()`, `CourseFullException` throw/catch, `Grade` constructor
- Xondamir: how signals and slots connect a button click to a C++ method
- Nodir: how `QMessageBox` displays exception messages, role-based view switching on login

---


---

## Suggested Order of Work

```
Week 1 — Core C++:
  C-01 User → C-02 Professor → C-04 Admin
  C-05 Student skeleton (Akmal) → C-05 Student methods (Malika starts)
  C-06 Course → C-09 Exceptions (Akbarali)
  C-07 Assignment skeleton (Akbarali) → C-07 methods + C-08 Grade (Rushana starts)

Week 2 — Integration + Qt:
  C-03 TeachingAssistant → C-10 main.cpp (Akmal + Akbarali)
  Q-01 setup → Q-02 Login → Q-03/Q-04/Q-05 (Xondamir + Nodir)
  Q-06 Student dashboard (Malika) → Q-07 Assignment viewer (Rushana)
  P-01/P-02/P-03 Slides (Malika + Rushana, ongoing)
```

---

## Notes for Individual Professor Review

| Member | Be ready to explain |
|---|---|
| Akmal | Virtual inheritance constructor chain in TA · `generateId()` · pure virtual in User |
| Akbarali | `saveToFile` / `loadFromFile` · `CourseFullException` throw and catch · Grade constructor |
| Xondamir | Qt signals and slots · role-based view switching · `QTableWidget` population |
| Nodir | `QMessageBox` for exceptions · login flow · `QDoubleSpinBox` for score input |
| Malika | `enrollCourse()` · `calculateGPA()` · `receiveGrade()` · Student dashboard Qt screen |
| Rushana | `calculateLetterGrade()` · `operator<` on Assignment · `operator>` on Grade · Assignment viewer Qt screen |
