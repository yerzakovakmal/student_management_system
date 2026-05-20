# Объяснение C++ концепций — Система управления курсами и оценками

Этот файл объясняет **почему** каждое решение в нашем проекте было принято именно так.
Простым языком, без лишнего жаргона. Если ты прочитал и понял — значит сможешь объяснить преподавателю.

---

## Содержание

1. [Модификаторы доступа — private, protected, public](#1-модификаторы-доступа)
2. [Virtual — виртуальные функции и виртуальное наследование](#2-virtual)
3. [Static — статические члены и методы](#3-static)
4. [Чисто виртуальные функции — абстрактные классы](#4-чисто-виртуальные-функции)
5. [Override — переопределение виртуальных функций](#5-override)
6. [Конструкторы и деструкторы](#6-конструкторы-и-деструкторы)
7. [Перегрузка операторов](#7-перегрузка-операторов)
8. [Наследование — одиночное, иерархическое, множественное](#8-наследование)
9. [Обработка исключений](#9-обработка-исключений)
10. [Указатели — владеющие и невладеющие](#10-указатели)
11. [Работа с файлами](#11-работа-с-файлами)
12. [Система идентификаторов — nextOrderNum и generateId](#12-система-идентификаторов)
13. [Зачем нужен каждый метод в классах](#13-зачем-нужен-каждый-метод)

---

## 1. Модификаторы доступа

Модификаторы доступа решают **кто имеет право читать или изменять данные**.
Представь это как этажи в здании университета:
- `public` — холл на первом этаже, куда может войти любой
- `protected` — преподавательская комната, куда входят только преподаватели и их ассистенты
- `private` — личный кабинет декана, куда никто кроме него не входит

---

### `private`

Только сам класс может читать и изменять эти данные.
Никто снаружи — даже дочерние классы — не может их трогать.

```cpp
class User {
private:
    static int totalUsers;   // только User.cpp может это менять
    static int nextOrderNum;
};
```

**Зачем в нашем проекте:**
`totalUsers` — счётчик всех пользователей в системе.
Если бы любой класс мог его менять напрямую, счётчик стал бы неправильным.
Мы закрываем его через `private` и даём безопасный способ читать через `getTotalUsers()`.

---

### `protected`

Сам класс И его дочерние классы могут обращаться к этим данным. Внешний код — нет.

```cpp
class User {
protected:
    string name;
    string email;
    string password;
};
```

**Зачем в нашем проекте:**
`Student`, `Professor`, `Admin` и `TeachingAssistant` — все они наследуют `User`.
Им нужно читать `name` и `email` напрямую — например, при выводе информации в `displayInfo()`.
Если бы поля были `private`, каждый дочерний класс вынужден был бы вызывать `getName()` каждый раз.
`protected` даёт дочерним классам прямой доступ, не открывая данные всем остальным.

---

### `public`

Любой код может вызвать эти методы — сам класс, дочерние классы и внешний код.

```cpp
class User {
public:
    string getName() const;
    bool login(string email, string password);
};
```

**Зачем в нашем проекте:**
`main.cpp`, Qt-экраны и другие классы должны уметь вызывать `getName()` и `login()`.
Это "парадный вход" в класс — безопасный, контролируемый способ взаимодействия с объектом.

---

### Правило большого пальца

> Данные (поля) → `private` или `protected`
> Методы, которые нужны всему миру → `public`
> Вспомогательные методы только для самого класса → `private`

---

## 2. Virtual

Слово `virtual` используется в двух совершенно разных ситуациях.
Важно не путать их.

---

### Виртуальные функции — "пусть дочерний класс решает, что произойдёт"

Представь, что у тебя есть список всех пользователей системы и ты вызываешь `displayInfo()` на каждом.
Без `virtual`, C++ решает, какой `displayInfo()` вызвать, основываясь на **типе указателя** (всегда `User`).
С `virtual`, C++ смотрит на **реальный объект** во время выполнения и вызывает правильную версию.

```cpp
// Без virtual:
User* u = new Student(...);
u->displayInfo();  // вызывает User::displayInfo() — НЕПРАВИЛЬНО

// С virtual:
virtual void displayInfo();  // объявлено в User
u->displayInfo();  // вызывает Student::displayInfo() — ПРАВИЛЬНО
```

**Реальный пример из нашего проекта:**

```cpp
vector<User*> allUsers;
allUsers.push_back(new Student(...));
allUsers.push_back(new Professor(...));
allUsers.push_back(new Admin(...));

for (User* u : allUsers) {
    cout << u->getRole() << endl;
    // Без virtual: три раза напечатает "User"
    // С virtual:   напечатает "Student", "Professor", "Admin"
}
```

Это называется **полиморфизм времени выполнения**.

---

### Виртуальный деструктор — "убедись, что запустится правильный деструктор"

```cpp
class User {
public:
    virtual ~User();
};
```

```cpp
User* u = new Student(...);
delete u;  // какой деструктор запустится?
```

Без `virtual`: запускается только `User::~User()`. Деструктор `Student` пропускается — утечка памяти.
С `virtual`: C++ сначала запускает `Student::~Student()`, потом `User::~User()` — правильный порядок.

**Правило:** Если в классе есть хотя бы одна `virtual` функция — деструктор тоже должен быть `virtual`.

---

### Виртуальное наследование — "один общий базовый класс, а не два"

Это важно только для `TeachingAssistant`, который наследует и от `Student`, и от `Professor`.
Без `virtual` у `TeachingAssistant` оказалось бы **два экземпляра `User`** — два поля `name`, два `email`.

```
Без virtual:              С virtual:

   User    User                 User
    |        |                 /    \
 Student  Professor        Student  Professor
      \  /                       \  /
  TeachingAssistant          TeachingAssistant
  (name ДВАЖДЫ)              (name ОДИН РАЗ)
```

```cpp
// Решение:
class Student   : virtual public User { ... };
class Professor : virtual public User { ... };

class TeachingAssistant : public Student, public Professor { ... };
```

Это называется решением **проблемы ромба (diamond problem)**.

---

## 3. Static

`static` означает, что член **принадлежит самому классу**, а не конкретному объекту.
Все объекты класса делят одну копию статического члена.

### Статические поля

```cpp
class Student : virtual public User {
private:
    static int nextOrderNum;   // общий для ВСЕХ объектов Student
    static int enrollmentYear;
};
```

**Аналогия:**
Представь завод, который собирает автомобили. У каждой машины свой цвет, модель, скорость.
Но на стене завода висит один счётчик: "Собрано машин: 4271".
Этот счётчик принадлежит заводу (классу), а не конкретной машине (объекту).

```cpp
Student s1("Али", ...);    // nextOrderNum = 1 → ID = "U2510001"
Student s2("Зара", ...);   // nextOrderNum = 2 → ID = "U2510002"
Student s3("Омар", ...);   // nextOrderNum = 3 → ID = "U2510003"
```

---

### Статические методы

```cpp
class User {
public:
    static int getTotalUsers();  // можно вызвать без объекта
};

cout << User::getTotalUsers();  // объект не нужен
```

**Почему `generateId()` статический:**
Когда запускается конструктор, объект ещё не существует.
Нельзя вызвать обычный метод на том, что ещё не создано.
Но ID нужен до того, как объект будет готов. Статические методы можно вызывать до создания объекта.

```cpp
Student::Student(string name, string email, string password)
    : User(name, email, password, "Student", Student::generateId())
    //                                       ^^ вызывается до создания объекта
{}
```

---

### Определение статических членов в `.cpp`

Распространённая ошибка — забыть определить статические члены в `.cpp`.

```cpp
// Student.hpp — только объявление:
static int nextOrderNum;

// Student.cpp — ОБЯЗАТЕЛЬНОЕ определение:
int Student::nextOrderNum = 1;
int Student::enrollmentYear = 25;
```

Без определения в `.cpp` компоновщик выдаст:
`undefined reference to Student::nextOrderNum`

---

## 4. Чисто виртуальные функции

Чисто виртуальная функция — это функция с `= 0` в конце.
Она означает: "Я объявляю функцию, но реализовывать её отказываюсь.
Каждый дочерний класс ОБЯЗАН предоставить свою реализацию."

```cpp
class User {
public:
    virtual string getRole()     const = 0;
    virtual void   displayInfo() const = 0;
};
```

Класс с хотя бы одной чисто виртуальной функцией — **абстрактный**.
Нельзя создать его объект напрямую:

```cpp
User u("Али", ...);  // ОШИБКА КОМПИЛЯЦИИ — User абстрактный
```

**Почему мы делаем `User` абстрактным:**
Отдельный объект "просто пользователь" не имеет смысла в нашей системе.
Каждый человек — либо Student, либо Professor, либо Admin, либо TA.
Абстрактность `User` программно это закрепляет.

---

## 5. Override

`override` — ключевое слово безопасности. Говорит компилятору проверить,
что в родителе действительно существует подходящий виртуальный метод.

```cpp
class Student : virtual public User {
public:
    string getRole() const override;
    void   displayInfo() const override;
};
```

**Зачем писать `override`?**

Без него опечатка становится молчаливой ошибкой:

```cpp
// User объявляет:
virtual string getRole() const;

// Student случайно пишет:
string getRole();  // забыли "const" — это НОВАЯ функция, не переопределение!
```

Без `override` — компилятор молчит, но полиморфизм не работает.
С `override` — сразу ошибка компиляции, находишь и исправляешь за секунды.

**Правило:** Всегда пиши `override` на каждом методе, который должен заменить виртуальный.

---

## 6. Конструкторы и деструкторы

### Конструктор

Запускается автоматически при создании объекта. Задаёт начальные значения полей и валидирует данные.

```cpp
User::User(string name, string email, string password,
           string role, string userId)
    : name(name), email(email), password(password),
      role(role), userId(userId)
{
    if (name.empty() || email.empty())
        throw InvalidUserException("Имя и email не могут быть пустыми");
    if (password.length() < 6)
        throw InvalidUserException("Пароль должен быть минимум 6 символов");
    ++totalUsers;
}
```

Часть `: name(name), email(email)...` — **список инициализации**. Быстрее чем присваивание в теле.
Валидация в конструкторе означает: невозможно создать некорректный объект.

---

### Деструктор

Запускается автоматически при уничтожении объекта.

```cpp
User::~User() {
    --totalUsers;  // поддерживаем счётчик актуальным
}
```

---

### Конструктор TeachingAssistant — самый сложный

При виртуальном наследовании правило таково:
**самый дочерний класс обязан вызвать конструктор виртуального базового класса напрямую.**

```cpp
TeachingAssistant::TeachingAssistant(
    string name, string email, string password,
    string studentId, string empId, string dept, double stipend)
    : User(name, email, password, "TeachingAssistant", studentId), // ОБЯЗАТЕЛЬНО первым
      Student(name, email, password),    // вызов User внутри Student — ИГНОРИРУЕТСЯ
      Professor(name, email, password, empId, dept), // вызов User внутри Professor — ИГНОРИРУЕТСЯ
      assignedCourse(nullptr),
      hoursWorked(0),
      stipend(stipend)
{}
```

Компилятор молча игнорирует вызовы `User(...)` внутри `Student` и `Professor`,
и использует только тот, что в `TeachingAssistant`. Гарантируется ровно один объект `User`.

---

## 7. Перегрузка операторов

### `operator==`

```cpp
bool User::operator==(const User& other) const {
    return userId == other.userId;
}
```

**Почему по `userId`, а не по `name`?**
Два студента могут иметь одинаковое имя. `userId` гарантированно уникален.
Сравнение по `userId` означает: "Это один и тот же человек в системе?"

---

### `operator!=`

```cpp
bool User::operator!=(const User& other) const {
    return !(*this == other);  // просто инвертируем operator==
}
```

Никогда не реализуй одну и ту же логику дважды. `!=` всегда противоположно `==`.

---

### `operator<<`

```cpp
ostream& operator<<(ostream& os, const User& u) {
    os << "[" << u.role << "] " << u.name
       << " <" << u.email << "> #" << u.userId;
    return os;
}
```

Это **friend-функция** — имеет доступ к приватным членам благодаря `friend` в заголовке.

**Почему возвращает `ostream&`?** Чтобы можно было цепочкой писать:
```cpp
cout << student1 << " и " << student2 << endl;  // работает
```

---

### `operator<` на Assignment

```cpp
bool Assignment::operator<(const Assignment& other) const {
    return dueDate < other.dueDate;
}
```

Даты в формате `"YYYY-MM-DD"` сортируются правильно строковым сравнением:
`"2025-03-01" < "2025-04-15"` — верно, март раньше апреля.

```cpp
sort(assignments.begin(), assignments.end());  // использует operator< автоматически
```

---

### `operator>` на Grade

```cpp
bool Grade::operator>(const Grade& other) const {
    return score > other.score;
}
```

Позволяет сравнивать кто набрал больше баллов: `if (gradeA > gradeB) ...`

---

## 8. Наследование

### Одиночное наследование

```cpp
class Student : virtual public User { ... };
```

`Student` автоматически имеет `name`, `email`, `login()`, `logout()` из `User` — без переписывания.

---

### Иерархическое наследование

```
         User
        / | \ \
  Student Prof Admin TA
```

Любой код, работающий с `User*`, автоматически работает с `Student*`, `Professor*`, `Admin*`.

```cpp
void printUserInfo(User* u) {
    cout << u->getRole() << ": " << u->getName() << endl;
}

printUserInfo(new Student(...));   // "Student: Алишер"
printUserInfo(new Professor(...)); // "Professor: Д-р Смит"
printUserInfo(new Admin(...));     // "Admin: Сара"
```

---

### Множественное наследование

```cpp
class TeachingAssistant : public Student, public Professor { ... };
```

Объект TA содержит всё из `Student` и всё из `Professor`.
Без виртуального наследования — проблема ромба. С `virtual` в родителях — решена.

---

## 9. Обработка исключений

### Бросание исключения

```cpp
void User::setEmail(string newEmail) {
    if (newEmail.find('@') == string::npos)
        throw InvalidUserException("Неверный формат email");
    email = newEmail;
}
```

`throw` немедленно останавливает текущую функцию и переходит к ближайшему `catch`.

---

### Перехват исключения

```cpp
try {
    student.enrollCourse(course);
} catch (CourseFullException& e) {
    cout << "Нельзя записаться: " << e.what() << endl;
} catch (EnrollmentException& e) {
    cout << "Ошибка записи: " << e.what() << endl;
}
```

**Почему сначала ловим более специфичный тип?**
`CourseFullException` наследует от `EnrollmentException`.
Если поставить `EnrollmentException` первым — он перехватит всё, включая `CourseFullException`.
Всегда: сначала самый специфичный, в конце самый общий.

---

### Иерархия наших исключений

```
runtime_error
└── EnrollmentException      (общая ошибка записи на курс)
    └── CourseFullException   (конкретно: курс переполнен)

out_of_range
└── GradeOutOfRangeException (оценка вне диапазона 0–100)

invalid_argument
└── InvalidUserException     (неверные данные пользователя)

runtime_error
└── FileIOException          (ошибка чтения/записи файла)
```

**Почему наследуемся от стандартных исключений?**
Код, который ловит `runtime_error`, автоматически поймает и наши исключения.
Они встраиваются в стандартную систему, не заменяя её.

---

## 10. Указатели

**Владеющий указатель** — объект создан через `new` этим классом, значит он же его `delete`.
**Невладеющий указатель** — класс получил указатель извне. Он НЕ должен его `delete`.

```cpp
class Course {
private:
    vector<Student*> enrolledStudents;  // невладеющий
    Professor* professor;               // невладеющий
};

Course::~Course() {
    // НЕ удаляем professor и студентов — мы их не создавали
}
```

**Двойное освобождение** — объект удаляется дважды — вызывает краш программы.

---

### Проверка на nullptr

```cpp
void TeachingAssistant::gradeAssignment(Student& s, double score) {
    if (!assignedCourse)
        throw runtime_error("У ассистента нет назначенного курса");
    s.receiveGrade(*assignedCourse, score);
}
```

---

## 11. Работа с файлами

### Сохранение в файл

```cpp
void Course::saveToFile(string filename) {
    ofstream file(filename);
    if (!file.is_open())
        throw FileIOException("Не удалось открыть файл: " + filename);

    file << courseId   << "\n"
         << courseName << "\n"
         << credits    << "\n"
         << capacity   << "\n";
    file.close();
}
```

`ofstream` — поток вывода в файл. Один поле на строку — легко читать обратно.

---

### Загрузка из файла

```cpp
Course Course::loadFromFile(string filename) {
    ifstream file(filename);
    if (!file.is_open())
        throw FileIOException("Не удалось открыть файл: " + filename);

    string courseId, courseName;
    int credits, capacity;

    getline(file, courseId);
    getline(file, courseName);
    file >> credits >> capacity;

    file.close();
    return Course(courseId, courseName, credits, capacity);
}
```

`ifstream` — поток ввода из файла.
`getline` читает всю строку включая пробелы.
`file >> variable` читает одно слово или число.

**Почему `loadFromFile` статический:**
Его вызывают без существующего объекта — он создаёт объект из файла.
`Course::loadFromFile("cs101.txt")` — без объекта, только имя класса.

---

## 12. Система идентификаторов

### Проблема

Каждый пользователь должен иметь уникальный читаемый ID вроде `U2510001`, присвоенный автоматически.

### Решение

```cpp
// Student.cpp
int Student::nextOrderNum   = 1;
int Student::enrollmentYear = 25;

string Student::generateId() {
    ostringstream oss;
    oss << "U"
        << setw(2) << setfill('0') << enrollmentYear   // "25"
        << setw(4) << setfill('0') << nextOrderNum++;  // "0001"
    return oss.str();  // "U2510001"
}
```

`setw(4)` — минимум 4 символа. `setfill('0')` — дополнять нулями.
Число 1 становится `"0001"`, а не просто `"1"`.

### Почему у каждого класса свой счётчик

- Счётчик Student → U2510001, U2510002...
- Счётчик Professor → P2510001, P2510002...
- Счётчик Admin → A2510001, A2510002...

Они независимы — нумерация каждой роли начинается с 1, коллизий нет.

---

## 13. Зачем нужен каждый метод

---

### Класс `User`

#### `login(string email, string password)`
**Что делает:** Сравнивает введённые данные с сохранёнными. Если совпадают — `true`. Иначе — `runtime_error`.

**Зачем в проекте:** Точка входа в систему. Qt-экран вызывает при нажатии "Войти"
и показывает `QMessageBox` с ошибкой если метод бросает исключение.

```cpp
bool User::login(string inputEmail, string inputPassword) {
    if (email == inputEmail && password == inputPassword) {
        cout << "Добро пожаловать, " << name << "!\n";
        return true;
    }
    throw runtime_error("Неверный email или пароль");
}
```

---

#### `logout()`
**Что делает:** Выводит сообщение о выходе из системы.

**Зачем в проекте:** В реальной системе здесь уничтожалась бы сессия и очищались токены.
У нас — основа для этой логики. Qt вызывает при нажатии "Выйти".

---

#### `updateProfile(string newName, string newEmail)`
**Что делает:** Меняет имя и email через сеттеры с валидацией.

**Зачем в проекте:** Студент мог ошибиться при вводе имени, профессор сменил email.
Использует сеттеры, а не прямое присваивание — валидация всегда срабатывает.

---

#### `changePassword(string oldPass, string newPass)`
**Что делает:** Проверяет старый пароль, потом меняет на новый. Бросает исключение если старый неверен.

**Зачем в проекте:** Безопасность. Пользователь должен знать текущий пароль чтобы изменить его.
Иначе любой, кто взял чужой телефон, мог бы сменить пароль.

---

#### `getTotalUsers()` — static
**Что делает:** Возвращает `totalUsers` — количество всех пользователей в памяти прямо сейчас.

**Зачем в проекте:** Admin вызывает в `generateSystemReport()`. В `main.cpp` проверяем
что объекты создаются и уничтожаются корректно. Демонстрирует работу статических членов.

---

#### `getRole()` — почему переопределяется в каждом классе

В `User` — чисто виртуальный (`= 0`). Компилятор **принудительно требует** реализацию в каждом потомке.

```cpp
// Student:
string getRole() const override { return "Student"; }
// Professor:
string getRole() const override { return "Professor"; }
// Admin:
string getRole() const override { return "Admin"; }
// TeachingAssistant:
string getRole() const override { return "TeachingAssistant"; }
```

**Зачем переопределять в КАЖДОМ:**
Когда у нас `vector<User*>` со всеми пользователями — `getRole()` единственный способ узнать кто есть кто.
В Qt это используется для показа правильного экрана после входа в систему:

```cpp
for (User* u : allUsers) {
    if (u->getRole() == "Student") {
        // показать студенческий экран
    } else if (u->getRole() == "Professor") {
        // показать экран профессора
    }
}
```

Если хотя бы один класс не переопределит — ошибка компиляции.

---

#### `displayInfo()` — почему переопределяется в каждом классе

У каждой роли разные поля для отображения:
```
Student:    name, email, studentId, gpa, enrolledCourses
Professor:  name, email, department, taughtCourses
Admin:      name, email, adminDepartment, permissions
```

Общий метод в `User` не знает о `gpa` студента или `department` профессора.
Каждый класс сам знает, что ему выводить.

---

### Класс `Student`

#### `enrollCourse(Course& c)`
**Что делает:** Добавляет курс в `enrolledCourses`. Проверяет `c.isFull()`. Бросает `CourseFullException` если переполнен.

**Зачем в проекте:** Студенты записываются на курсы. Без этого метода нет способа
связать студента с курсами и проверить лимит мест.

---

#### `dropCourse(Course& c)`
**Что делает:** Удаляет курс из `enrolledCourses`. Бросает `EnrollmentException` если студент не записан.

**Зачем в проекте:** Студент может передумать или выбрать другой курс.
Метод чистит связь: у студента из вектора, у курса из ростера.

---

#### `calculateGPA()`
**Что делает:** Проходит по всем оценкам, суммирует, делит на количество, обновляет и возвращает `gpa`.

**Зачем в проекте:** GPA нужен для транскрипта, отчётов профессора, Qt-дашборда.
Пересчитываем динамически — GPA всегда актуален.

```cpp
double Student::calculateGPA() {
    if (grades.empty()) return 0.0;
    double total = 0.0;
    for (const Grade& g : grades)
        total += g.getScore();
    gpa = total / grades.size();
    return gpa;
}
```

---

#### `receiveGrade(Course& c, double score)`
**Что делает:** Принимает оценку от профессора или TA. Создаёт объект `Grade` и добавляет в `grades`.

**Зачем в проекте:** Студент не ставит себе оценки сам. Это "приёмный лоток".
`Professor::assignGrade()` вызывает именно `student.receiveGrade(...)`.

---

#### `viewTranscript()`
**Что делает:** Строит форматированный отчёт по всем курсам и оценкам через `ostringstream`, возвращает строку.

**Зачем в проекте:** Официальный документ успеваемости. В Qt Малика отображает это в текстовом диалоге.
Возвращает строку, а не печатает — Qt сам отобразит.

---

#### `submitAssignment(Assignment& a)`
**Что делает:** Выводит подтверждение что задание было сдано студентом.

**Зачем в проекте:** Фиксирует факт сдачи. В полноценной системе здесь бы загружался файл работы.
Для нашего проекта — подтверждение в консоли.

---

### Класс `Professor`

#### `createCourse(Course* course)`
**Что делает:** Добавляет указатель на курс в `taughtCourses`.

**Зачем в проекте:** Профессор должен знать, какие курсы он ведёт.
Admin вызывает при назначении профессора. Позволяет перебирать свои курсы.

---

#### `assignGrade(Student& student, Course& course, double score)`
**Что делает:** Проверяет диапазон оценки, вызывает `student.receiveGrade(...)`.

**Зачем в проекте:** Только профессор или TA имеет право ставить оценки.
Централизует право и добавляет валидацию.

---

#### `viewRoster(Course& course)`
**Что делает:** Возвращает `vector<Student*>` студентов курса. **Не печатает** — возвращает данные.

**Зачем в проекте:** Профессор должен видеть кто учится на его курсе.
Переименовано из `manageRoster()` и изменён возвращаемый тип для совместимости с Qt.

**Почему возвращает данные, а не печатает:**
```cpp
// НЕПРАВИЛЬНО для Qt:
void manageRoster(Course& c) {
    for (auto* s : c.getEnrolledStudents())
        cout << s->getName() << "\n";  // Qt не может использовать cout
}

// ПРАВИЛЬНО:
vector<Student*> viewRoster(Course& c) {
    return c.getEnrolledStudents();  // Qt заполняет таблицу этим вектором
}
```

---

#### `removeStudentFromCourse(Student& student, Course& course)`
**Что делает:** Вызывает `course.removeStudent(student)`.

**Зачем в проекте:** Только профессор или admin могут отчислить студента с курса.
Это не то же что `student.dropCourse()` — здесь инициатива от профессора.

---

#### `generateReport(Course& course) const`
**Что делает:** Строит текстовый отчёт — количество студентов, GPA каждого. Возвращает `string`.

**Зачем в проекте:** Академические отчёты. В Qt отображается в текстовом виджете.
Возвращает строку — не печатает — чтобы и Qt и консоль использовали одинаково.

---

#### `assignTA(TeachingAssistant& ta, Course& course)`
**Что делает:** Вызывает `ta.setAssignedCourse(&course)`.

**Зачем в проекте:** Профессор решает, кто будет ассистировать на каком курсе.
После этого TA может выставлять оценки и публиковать объявления.

---

### Класс `TeachingAssistant`

#### `setAssignedCourse(Course* course)`
**Что делает:** Сохраняет указатель на курс в `assignedCourse`. Проверяет что указатель не `nullptr`.

**Зачем в проекте:** TA должен знать свой курс. Все методы TA проверяют это поле
и бросают исключение если `nullptr`. Без курса TA ничего не может.

---

#### `gradeAssignment(Student& student, double score)`
**Что делает:** Валидирует оценку, проверяет курс, вызывает `student.receiveGrade(...)`.

**Зачем в проекте:** TA часто проверяют работы вместо профессора.
Дает TA право ставить оценки — но только на своём курсе.

---

#### `holdOfficeHours(int durationMins)`
**Что делает:** Вызывает `logHours(durationMins / 60)`, выводит подтверждение.

**Зачем в проекте:** TA получают стипендию частично за консультации.
Отслеживание часов позволяет считать нагрузку TA.

---

#### `postAnnouncement(string msg)`
**Что делает:** Вызывает `assignedCourse->addAnnouncement(msg)`. Проверяет что курс назначен.

**Зачем в проекте:** TA публикуют напоминания о дедлайнах. Только TA своего курса.

---

#### `assistProfessor(Professor& prof)`
**Что делает:** Выводит сообщение что TA ассистирует данному профессору.

**Зачем в проекте:** Показывает связь TA с профессором. В более полной версии
здесь была бы передача задач от профессора ассистенту.

---

#### `viewCourseRoster()`
**Что делает:** Выводит список студентов назначенного курса. Бросает если курс не назначен.

**Зачем в проекте:** TA нужно видеть студентов своего курса для работы с ними.

---

### Класс `Admin`

#### `createStudent(...)` / `createProfessor(...)`
**Что делают:** Создают объект `new Student(...)` или `new Professor(...)`, возвращают указатель.

**Зачем в проекте:** В реальных университетах студенты не регистрируются сами.
Академический отдел создаёт аккаунты. Только Admin может создавать пользователей —
защита от несанкционированного добавления в систему.

---

#### `deleteUser(User* user)`
**Что делает:** Выводит подтверждение удаления. Вызывающий код отвечает за фактический `delete`.

**Зачем в проекте:** Студент отчислен или профессор уволен — их аккаунты удаляются.
Только Admin имеет право на это действие.

---

#### `viewAllStudents(vector<Student*>&)` / `viewAllProfessors(vector<Professor*>&)`
**Что делают:** Выводят или возвращают полный список пользователей роли.

**Зачем в проекте:** Admin видит всех пользователей системы. Qt-панель Admin
заполняет таблицу этими данными.

---

#### `assignProfessorToCourse(Professor& prof, Course& course)`
**Что делает:** Вызывает `prof.createCourse(&course)` — связывает профессора с курсом.

**Зачем в проекте:** Только Admin назначает кто преподаёт какой курс.
Это организационное решение, а не академическое.

---

#### `generateSystemReport()`
**Что делает:** Возвращает строку с общей статистикой — пользователи, курсы, оценки.

**Зачем в проекте:** Руководству университета нужен обзор. Admin видит всё.
Вызывает `User::getTotalUsers()` и `Course::courseCount`.

---

#### `hasPermission(string perm) const`
**Что делает:** Проверяет есть ли строка в векторе `permissions`.

**Зачем в проекте:** Не все Admin одинаковые. IT управляет пользователями,
Академический отдел управляет курсами. Метод позволяет проверить права перед действием.

---

### Класс `Course`

#### `addStudent(Student* s)`
**Что делает:** Проверяет `isFull()`, добавляет студента. Бросает `CourseFullException` если переполнен.

**Зачем в проекте:** Курсы имеют лимит мест. Без проверки в аудиторию на 30 мест
могло бы записаться 200 студентов.

---

#### `isFull() const`
**Что делает:** Возвращает `enrolledStudents.size() >= capacity`.

**Зачем в проекте:** Используется в `addStudent()` и в `Student::enrollCourse()`.
Отдельный метод вместо прямого сравнения — если логика изменится, менять только в одном месте.

---

#### `saveToFile()` / `loadFromFile()`
**Что делают:** Сохраняют и загружают данные курса из текстового файла.

**Зачем в проекте:** Без файлов данные теряются при закрытии программы.
`loadFromFile` статический — создаёт объект из файла без предварительного объекта.

---

### Класс `Grade`

#### `calculateLetterGrade()` — private
**Что делает:** Преобразует числовой балл в букву по шкале A/B/C/D/F.

**Зачем в проекте:** Буква должна всегда соответствовать числу.
Вызывается в конструкторе И в `setScore()`. Если забыть второй вызов — буква не обновится.

**Почему `private`:** Никто снаружи не должен вызывать это напрямую.
Внешний код вызывает `setScore()`, который сам пересчитывает букву.

---

#### `setScore(double newScore)`
**Что делает:** Валидирует новый балл, обновляет `score`, вызывает `calculateLetterGrade()`.

**Зачем в проекте:** Профессор может исправить оценку после апелляции.
Без этого метода пришлось бы удалять и создавать новый объект `Grade`.

---

### Класс `Assignment`

#### `operator<(const Assignment& other) const`
**Что делает:** Сравнивает даты сдачи строковым сравнением.

**Зачем в проекте:** Позволяет сортировать задания по дедлайну автоматически через `std::sort`.
Рушана использует это в Qt-экране просмотра заданий — они всегда отображаются по порядку.

---

*Конец файла объяснений.*
*Если что-то всё ещё непонятно — спрашивай, для этого файл и существует.*
