#include "log_msg.hpp"

void LogError(const std::string& msg) {
  std::cerr << "\033[31m[ ERROR ] \033[0m" << msg << std::endl;
}

void LogSuccess(const std::string& msg) {
  std::cout << "\033[32m[ OK ]    \033[0m" << msg << std::endl;
}

void LogInfo(const std::string& msg) {
  std::cout << "\033[33m[ INFO ]  \033[0m" << msg << std::endl;
}