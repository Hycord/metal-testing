#include <string>
#include <sstream>

#pragma once
std::string ReadFile(std::string location);
std::stringstream ReadFileStream(std::string location);
bool Exists(std::string location);