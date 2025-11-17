#include "sql_handling.hpp"

Database::Database(const std::string& db_name)
    : db_(nullptr), db_filename_(db_name) {}

Database::~Database() {
  if (db_) {
    sqlite3_close(db_);
    LogInfo("Database connection closed.");
  }
}

bool Database::Open() {
  int rc = sqlite3_open(db_filename_.c_str(), &db_);
  if (rc != SQLITE_OK) {
    LogError("Cannot open database: " + std::string(sqlite3_errmsg(db_)));
    sqlite3_close(db_);
    db_ = nullptr;
    return false;
  }
  LogSuccess("Database opened successfully: " + db_filename_);
  return true;
}

bool Database::Exec(const std::string& sql) {
  char* zErrMsg = nullptr;
  int rc = sqlite3_exec(db_, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    LogError("SQL error: " + std::string(zErrMsg));
    sqlite3_free(zErrMsg);
    return false;
  }
  return true;
}

bool Database::CreateTable() {
  const std::string sql =
      "CREATE TABLE IF NOT EXISTS STUDENTS ("
      "ID    INTEGER PRIMARY KEY AUTOINCREMENT,"
      "NAME  TEXT    NOT NULL,"
      "GRADE REAL    NOT NULL);";
  if (!Exec(sql)) {
    LogError("Failed to create 'STUDENTS' table.");
    return false;
  }
  LogInfo("Table 'STUDENTS' is ready.");
  return true;
}

bool Database::AddStudent(const std::string& name, double grade) {
  const std::string sql = "INSERT INTO STUDENTS (NAME, GRADE) VALUES (?, ?);";
  sqlite3_stmt* stmt = nullptr;
  int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    LogError("Failed to prepare statement: " +
             std::string(sqlite3_errmsg(db_)));
    return false;
  }

  sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_double(stmt, 2, grade);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    LogError("Failed to execute statement: " +
             std::string(sqlite3_errmsg(db_)));
    sqlite3_finalize(stmt);
    return false;
  }

  sqlite3_finalize(stmt);
  LogSuccess("Added student: " + name);
  return true;
}

bool Database::RemoveStudent(int id) {
  const std::string sql = "DELETE FROM STUDENTS WHERE ID = ?;";
  sqlite3_stmt* stmt = nullptr;
  int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    LogError("Failed to prepare statement: " +
             std::string(sqlite3_errmsg(db_)));
    return false;
  }

  // Bind the ID
  sqlite3_bind_int(stmt, 1, id);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    LogError("Failed to execute statement: " +
             std::string(sqlite3_errmsg(db_)));
    sqlite3_finalize(stmt);
    return false;
  }

  if (sqlite3_changes(db_) == 0) {
    LogError("Could not remove student: ID " + std::to_string(id) +
             " not found.");
    sqlite3_finalize(stmt);
    return false;
  }

  sqlite3_finalize(stmt);
  LogSuccess("Removed student with ID: " + std::to_string(id));
  return true;
}

bool Database::UpdateStudentGrade(int id, double new_grade) {
  const std::string sql = "UPDATE STUDENTS SET GRADE = ? WHERE ID = ?;";
  sqlite3_stmt* stmt = nullptr;
  int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    LogError("Failed to prepare statement: " +
             std::string(sqlite3_errmsg(db_)));
    return false;
  }

  sqlite3_bind_double(stmt, 1, new_grade);
  sqlite3_bind_int(stmt, 2, id);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    LogError("Failed to execute statement: " +
             std::string(sqlite3_errmsg(db_)));
    sqlite3_finalize(stmt);
    return false;
  }

  if (sqlite3_changes(db_) == 0) {
    LogError("Could not update grade: ID " + std::to_string(id) +
             " not found.");
    sqlite3_finalize(stmt);
    return false;
  }

  sqlite3_finalize(stmt);
  LogSuccess("Updated grade for student ID: " + std::to_string(id));
  return true;
}

bool Database::GetStudent(int id, Student* out_student) {
  const std::string sql = "SELECT ID, NAME, GRADE FROM STUDENTS WHERE ID = ?;";
  sqlite3_stmt* stmt = nullptr;
  int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    LogError("Failed to prepare statement: " +
             std::string(sqlite3_errmsg(db_)));
    return false;
  }

  sqlite3_bind_int(stmt, 1, id);

  bool found = false;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    out_student->id = sqlite3_column_int(stmt, 0);
    out_student->name =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    out_student->grade = sqlite3_column_double(stmt, 2);
    found = true;
  }

  sqlite3_finalize(stmt);
  return found;
}

bool Database::GetAllStudents(std::vector<Student>* out_students) {
  out_students->clear();
  const std::string sql = "SELECT ID, NAME, GRADE FROM STUDENTS;";
  sqlite3_stmt* stmt = nullptr;
  int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    LogError("Failed to prepare statement: " +
             std::string(sqlite3_errmsg(db_)));
    return false;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Student s;
    s.id = sqlite3_column_int(stmt, 0);
    s.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    s.grade = sqlite3_column_double(stmt, 2);
    out_students->push_back(s);
  }

  sqlite3_finalize(stmt);
  return true;
}