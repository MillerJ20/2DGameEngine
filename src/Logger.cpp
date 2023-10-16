#include "Logger.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

using namespace std;

std::string getDateTimeString() {
  time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
  string output(30, '\0');
  strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", localtime(&now));
  return output;
}

void Logger::Log(const std::string &message) {
  string output = "LOG: [" + getDateTimeString() + "] " + message;
  cout << "\x1B[32m" << output << "\033[0m" << endl;
}

void Logger::Err(const std::string &message) {
  string output = "LOG: [" + getDateTimeString() + "] " + message;
  cerr << "\x1B[32m" << output << "\033[0m" << endl;
}
