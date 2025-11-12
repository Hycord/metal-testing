#include "LogManager.h"
#include <iostream>

void LogManager::Log(const char *message)
{
    // Simple log to console
    std::cout << message << std::endl;
}