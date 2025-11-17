cmake_minimum_required(VERSION 3.10)
project(university_db CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SQLITE3 REQUIRED sqlite3)

add_executable(
  university_app
  main.cpp
  log_msg.cpp
  sql_handling.cpp
)

target_link_libraries(university_app PRIVATE ${SQLITE3_LIBRARIES})

target_include_directories(university_app PRIVATE ${SQLITE3_INCLUDE_DIRS})

set_target_properties(
  university_app
  PROPERTIES CXX_EXTENSIONS OFF
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(university_app PRIVATE -Wall -Wextra -Wpedantic)
endif()