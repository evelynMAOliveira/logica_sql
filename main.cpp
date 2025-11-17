#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "log_msg.hpp"
#include "sql_handling.hpp"

void PrintMenu() {
  std::cout << "\n--- University Database Menu ---\n";
  std::cout << "1. Add Student\n";
  std::cout << "2. Remove Student\n";
  std::cout << "3. Update Student Grade\n";
  std::cout << "4. Find Student by ID\n";
  std::cout << "5. View All Students\n";
  std::cout << "0. Exit\n";
  std::cout << "Enter your choice: ";
}

int GetUserChoice() {
  int choice = -1;
  while (!(std::cin >> choice)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    LogError("Invalid input. Please enter a number.");
    std::cout << "Enter your choice: ";
  }
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return choice;
}

double GetUserDouble(const std::string& prompt) {
  double value = 0.0;
  std::cout << prompt;
  while (!(std::cin >> value)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    LogError("Invalid input. Please enter a number (e.g., 85.5).");
    std::cout << prompt;
  }
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return value;
}

std::string GetUserString(const std::string& prompt) {
  std::string input;
  while (true) {
    std::cout << prompt;
    std::getline(std::cin, input);
    if (!input.empty()) {
      break;
    }
    LogError("Input cannot be empty.");
  }
  return input;
}

void HandleAddStudent(Database* db) {
  LogInfo("--- Add New Student ---");
  std::string name = GetUserString("Enter student name: ");
  double grade = GetUserDouble("Enter student grade (0-100): ");
  db->AddStudent(name, grade);
}

void HandleRemoveStudent(Database* db) {
  LogInfo("--- Remove Student ---");
  int id = GetUserChoice();  // Re-using GetUserChoice for integer input
  db->RemoveStudent(id);
}

void HandleUpdateGrade(Database* db) {
  LogInfo("--- Update Student Grade ---");
  std::cout << "Enter ID of student to update: ";
  int id = GetUserChoice();
  double grade = GetUserDouble("Enter new grade (0-100): ");
  db->UpdateStudentGrade(id, grade);
}

void PrintStudent(const Student& s) {
  std::cout << "| " << std::setw(4) << s.id << " | " << std::setw(20)
            << std::left << s.name << std::right << " | " << std::setw(6)
            << std::fixed << std::setprecision(2) << s.grade << " |\n";
}

void PrintStudentHeader() {
  std::cout << "+-" << std::string(4, '-') << "-+-" << std::string(20, '-')
            << "-+-" << std::string(6, '-') << "-+\n";
  std::cout << "| " << std::setw(4) << "ID"
            << " | " << std::setw(20) << std::left << "Name" << std::right
            << " | " << std::setw(6) << "Grade"
            << " |\n";
  std::cout << "+-" << std::string(4, '-') << "-+-" << std::string(20, '-')
            << "-+-" << std::string(6, '-') << "-+\n";
}

void HandleFindStudent(Database* db) {
  LogInfo("--- Find Student ---");
  std::cout << "Enter ID of student to find: ";
  int id = GetUserChoice();
  Student s;
  if (db->GetStudent(id, &s)) {
    PrintStudentHeader();
    PrintStudent(s);
  } else {
    LogError("Student with ID " + std::to_string(id) + " not found.");
  }
}

void HandleViewAllStudents(Database* db) {
  LogInfo("--- All Students ---");
  std::vector<Student> students;
  if (!db->GetAllStudents(&students)) {
    LogError("Could not retrieve students.");
    return;
  }
  if (students.empty()) {
    LogInfo("No students in the database.");
    return;
  }

  PrintStudentHeader();
  for (const auto& s : students) {
    PrintStudent(s);
  }
}

int main() {
  // Create and open the database
  Database db("university.db");
  if (!db.Open()) {
    return 1;
  }
  if (!db.CreateTable()) {
    return 1;
  }

  bool running = true;
  while (running) {
    PrintMenu();
    int choice = GetUserChoice();
    switch (choice) {
      case 1:
        HandleAddStudent(&db);
        break;
      case 2:
        HandleRemoveStudent(&db);
        break;
      case 3:
        HandleUpdateGrade(&db);
        break;
      case 4:
        HandleFindStudent(&db);
        break;
      case 5:
        HandleViewAllStudents(&db);
        break;
      case 0:
        running = false;
        LogInfo("Exiting program.");
        break;
      default:
        LogError("Invalid choice. Please try again.");
        break;
    }
  }

  return 0;
}