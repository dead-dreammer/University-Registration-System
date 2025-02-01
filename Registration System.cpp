#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <ctime>
#include <algorithm>

using namespace std;

class Student {
public:
    int student_id;
    string major;
    vector<string> courses;
    int academic_year;

        // Default constructor
    Student() : student_id(0), major(""), academic_year(1) {}

    // Constructor to initialize Student attributes
    Student(int id, string maj, int year) :
        student_id(id), major(maj), academic_year(year) {}
};

class Course {
public:
    string course_code;
    string name;
    int max_cap;
    string roomType;
    vector<Student*> enrolledStudents;
    vector<int> scheduledTimeSlots;

    // Constructor to initialize Course attributes
    Course(string code, string name, int cap, string type) 
        : course_code(code), name(name), max_cap(cap), roomType(type) {}

    // Method to enroll a student in the course
    bool enrollStudent(Student* student) {
        if (enrolledStudents.size() < max_cap) {
            enrolledStudents.push_back(student);
            student->courses.push_back(course_code);
            return true;
        }
        return false;
    }
};

class Room {
public:
    int room_num;
    string room_type;
    int capacity;
    vector<int> availableTimeSlots;
    string specialEquipment;

    // Constructor to initialize Room attributes
    Room(int num, string type, int cap, vector<int> slots, string equipment) 
        : room_num(num), room_type(type), capacity(cap), availableTimeSlots(slots), specialEquipment(equipment) {}
};

struct RegistrationRequest {
    Student* student;
    Course* course;
    time_t timestamp;

    // Constructor to initialize RegistrationRequest attributes
    RegistrationRequest(Student* s, Course* c, time_t time) : student(s), course(c), timestamp(time) {}
};

// Comparator struct for priority queue ordering of RegistrationRequests
struct CompareRequests {
    bool operator()(const RegistrationRequest& r1, const RegistrationRequest& r2) {
        // Higher priority for students with a higher academic year
        if (r1.student->academic_year != r2.student->academic_year) {
            return r1.student->academic_year < r2.student->academic_year;
        }
        // If same academic year, prioritize by earlier timestamp
        return r1.timestamp > r2.timestamp;
    }
};

class SchedulingSystem {
private:
    vector<Course> courses;
    vector<Room> rooms;
    unordered_map<int, Student> students;
    priority_queue<RegistrationRequest, vector<RegistrationRequest>, CompareRequests> requestQueue;

public:
    // Add a course to the system
    void addCourse(const Course& course) { courses.push_back(course); }

    // Add a room to the system
    void addRoom(const Room& room) { rooms.push_back(room); }

    // Add a student to the system
    void addStudent(const Student& student) { students[student.student_id] = student; }

    // Submit a registration request for a student to enroll in a course
    void submitRegistrationRequest(Student* student, Course* course) {
        time_t now = time(0); // Current time as timestamp
        requestQueue.push(RegistrationRequest(student, course, now));
    }

    // Process all registration requests
    void processRegistrations() {
        while (!requestQueue.empty()) {
            RegistrationRequest request = requestQueue.top();
            requestQueue.pop();
            allocateCourse(request.student, request.course);
        }
    }

private:
    // Allocate a course to a student, assigning an appropriate room if available
    void allocateCourse(Student* student, Course* course) {
        for (auto& room : rooms) {
            // Check if the room type and capacity match the course's requirements
            if (room.room_type == course->roomType && room.capacity >= course->max_cap) {
                // Try to find an available time slot in the room that doesn't conflict with the course
                for (int slot : room.availableTimeSlots) {
                    if (find(course->scheduledTimeSlots.begin(), course->scheduledTimeSlots.end(), slot) == course->scheduledTimeSlots.end()) {
                        course->scheduledTimeSlots.push_back(slot); // Assign time slot to course
                        if (course->enrollStudent(student)) {       // Enroll student in the course
                            cout << "Student " << student->student_id << " enrolled in " << course->name 
                                 << " in room " << room.room_num << endl;
                        }
                        return; // Exit after successful allocation
                    }
                }
            }
        }
        cout << "No suitable room available for course " << course->name << endl;
    }

    // Optimizes scheduling by processing requests from the priority queue
    void optimizeSchedule() {
        while (!requestQueue.empty()) {
            RegistrationRequest req = requestQueue.top(); // Get the highest-priority request
            requestQueue.pop();

            Course* course = req.course;
            Student* student = req.student;

            bool roomAssigned = false;
            for (Room& room : rooms) {
                if (room.room_type == course->roomType && room.capacity >= course->max_cap) {
                    if (!room.availableTimeSlots.empty()) {
                        int timeSlot = room.availableTimeSlots.back();
                        room.availableTimeSlots.pop_back();
                        course->scheduledTimeSlots.push_back(timeSlot);
                        course->enrollStudent(student);
                        roomAssigned = true;
                        break;
                    }
                }
            }

            if (!roomAssigned) {
                cout << "Could not assign room for course: " << course->name << endl;
            }
        }
    }
};

int main() {
    // Initialize SchedulingSystem and add students, courses, and rooms as needed
    SchedulingSystem system;

    // Example data
    Student s1(1, "Computer Science", 3);
    Student s2(2, "Mathematics", 2);

    system.addStudent(s1);
    system.addStudent(s2);

    Course c1("CS101", "Data Structures", 30, "Lab");
    system.addCourse(c1);

    vector<int> timeSlots = {9, 10, 11}; // Example times
    Room r1(101, "Lab", 30, timeSlots, "Projector");
    system.addRoom(r1);

    system.submitRegistrationRequest(&s1, &c1);
    system.submitRegistrationRequest(&s2, &c1);

    system.processRegistrations();

    return 0;
}
