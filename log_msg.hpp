#ifndef LOG_MSG_HPP
#define LOG_MSG_HPP

#include <iostream>
#include <string>

void LogError(const std::string& msg);
void LogSuccess(const std::string& msg);
void LogInfo(const std::string& msg);

#endif  // LOG_MSG_HPP