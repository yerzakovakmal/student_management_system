=====================================================
      UNIVERSITY MANAGEMENT SYSTEM (CLI VERSION)
=====================================================

Overview:
---------
This is a C++ based University Management System that allows 
Admins, Professors, and Students to manage academic records, 
course enrollments, schedules, and financial balances.

Compilation and Execution:
--------------------------
To compile the project:
g++ -o university_system main.cpp src/*.cpp

To run:
./university_system

Login Methods:
---------------
The system uses different identification methods based on the user's role:

1. Admin Login: 
   - Uses: EMAIL
   - Identifier: 'admin'
   - Password: 'admin'

2. Student Login:
   - Uses: SYSTEM ID (e.g., S26-0001)
   - Search: Scans database (students.dat)
   - Demo Example: ID: S26-0004 | Password: akmal123

3. Professor Login:
   - Uses: SYSTEM ID (e.g., P26-0001)
   - Search: Scans database (professors.dat)
   - Demo Example: ID: P26-0002 | Password: suvanov123

To locate IDs for accounts you have created, use the 'Save system report' 
option (Choice 16) in the Admin Panel to view 'admin_report.txt'.

Role Features:
--------------
1. Admin Panel:
   - Manage Users (Create/Remove Students & Professors)
   - Manage Courses (Add/Remove/Assign Profs/Enroll Students)
   - Assign Grades
   - Generate System Reports

2. Professor Panel:
   - View assigned courses and student rosters
   - Create assignments and post announcements
   - Grade students

3. Student Panel:
   - View enrolled courses and grades
   - View personalized Timetable
   - Check Tuition Balance
   - Print Academic Transcript

Data Persistence:
-----------------
All data is saved in binary format in:
- students.dat
- professors.dat
- courses.dat

Note: Students and Professors must now use their unique System IDs 
(e.g., S26-0001, P26-0001) to log in. Admins continue to use Email.
