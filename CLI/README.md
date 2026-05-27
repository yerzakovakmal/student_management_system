# 🎓 University Management System (CLI)

This is a C++ based University Management System that allows Admins, Professors, and Students to manage academic records, course enrollments, schedules, and financial balances.

## 🚀 Overview
The system allows for comprehensive management of academic life, including:
*   **User Management** (Students & Professors)
*   **Course Coordination** (Schedules & Capacity)
*   **Financial Tracking** (Tuition & Balances)
*   **Academic Records** (Grading & Transcripts)

---

## 🛠 Compilation & Execution

### Windows (MinGW/GCC)
```bash
g++ -o university_system main.cpp src/*.cpp
./university_system
```

### MacOS / Linux
```bash
clang++ -o university_system main.cpp src/*.cpp
./university_system
```

---

## 🔐 Login Methods

Access to the system is partitioned by role. Each role uses a specific identifier type for authentication:

| Role | Identifier Type | Search Logic |
| :--- | :--- | :--- |
| **Admin** | **Email** | Scans hardcoded administrator credentials. |
| **Student** | **System ID** | Scans runtime database (`students.dat`) or demo fallback. |
| **Professor** | **System ID** | Scans runtime database (`professors.dat`) or demo fallback. |

### 🔑 Default Demo Credentials

| Role | Identifier (ID/Email) | Password |
| :--- | :--- | :--- |
| **Admin** | `admin` | `admin` |
| **Professor** | `P26-0002` | `suvanov123` |
| **Student** | `S26-0004` | `akmal123` |

> 💡 **Tip:** To find IDs for newly created users, check the **System Report** (`admin_report.txt`) generated via the Admin Panel.

---

## 📋 Role Features

### 🛠 Admin Panel
- **User Control:** Create or remove student and professor accounts.
- **Course Studio:** Add/remove courses, assign professors, and set schedules.
- **Enrolment:** Manage student course registrations and capacity limits.
- **Analytics:** Generate comprehensive system reports.

### 👨‍🏫 Professor Panel
- **Classroom Management:** View rosters for all assigned courses.
- **Engagement:** Create assignments and post context-specific announcements.
- **Evaluation:** Grade students directly within the portal.

### 🎓 Student Panel
- **Academia:** View enrolled courses and real-time grades.
- **Schedule:** Access a personalized visual timetable (`Mon, Wed @ 10:00`).
- **Finance:** Monitor tuition balance, payments, and outstanding debt.
- **Records:** Generate and print a formal Academic Transcript.

---

## 💾 Data Persistence
The system automatically manages data across sessions using binary serialization stored in:
- 📁 `students.dat`
- 📁 `professors.dat`
- 📁 `courses.dat`

---

*Developed by team Syrniki as a Group Project Work assigned @ IUT OOP2 Course*
