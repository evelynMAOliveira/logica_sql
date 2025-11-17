#ifndef SQL_HANDLING_HPP
#define SQL_HANDLING_HPP

#include <sqlite3.h>

#include <iostream>
#include <string>
#include <vector>

#include "log_msg.hpp"

// A struct to represent our Student data.
// This is cleaner than passing around raw values.
struct Student {
  int id;
  std::string name;
  double grade;
};

// A class to manage the database connection and operations.
class Database {
 public:
  // Constructor: Takes the database filename
  explicit Database(const std::string& db_name);

  // Destructor: Cleans up the database connection
  ~Database();

  // --- Core Functions ---

  // Opens (or creates) the database file.
  // Returns true on success, false on failure.
  bool Open();

  // Creates the necessary 'STUDENTS' table if it doesn't exist.
  // Returns true on success, false on failure.
  bool CreateTable();

  // --- Student Management Functions ---

  // Adds a new student to the table.
  bool AddStudent(const std::string& name, double grade);

  // Removes a student from the table by their ID.
  bool RemoveStudent(int id);

  // Updates the grade for a student by their ID.
  bool UpdateStudentGrade(int id, double new_grade);

  // Retrieves a single student's data by their ID.
  // Fills 'out_student' with data if found.
  // Returns true if student was found, false otherwise.
  bool GetStudent(int id, Student* out_student);

  // Retrieves all students from the table.
  // Fills 'out_students' vector with data.
  // Returns true on success, false on failure.
  bool GetAllStudents(std::vector<Student>* out_students);

 private:
  // Helper function to execute simple SQL commands
  // (like CREATE TABLE) that don't return data.
  bool Exec(const std::string& sql);

  sqlite3* db_;              // The SQLite database connection handle
  std::string db_filename_;  // The name of the database file
};

#endif  // SQL_HANDLING_HPP