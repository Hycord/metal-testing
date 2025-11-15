#include "utils/FileReader.h"
#include "core/LogManager.h"
#include <fstream>
std::stringstream ReadFileStream(std::string location)
{
    LOG_START("ReadFileStream %s", location.c_str());
    std::ifstream file(location);
    std::stringstream data;

    if (file.is_open())
    {
       data << file.rdbuf();
    }

    LOG_FINISH("ReadFileStream %s", location.c_str());
    return data;
};

std::string ReadFile(std::string location)
{
    LOG_START("ReadFile %s", location.c_str());
    std::ifstream file(location);
    std::string data;

    if (file.is_open())
    {
        
        std::string line;
        while (std::getline(file, line))
        {
            data += line;
            data += '\n'; 
        }
        file.close(); 
    }

    LOG_FINISH("ReadFile %s (size=%zu)", location.c_str(), data.size());
    return data;
}

bool Exists(const std::string name)
{
    LOG_STEP("Exists check: %s", name.c_str());
    if (FILE *file = fopen(name.c_str(), "r"))
    {
        fclose(file);
        LOG_STEP("Exists: true %s", name.c_str());
        return true;
    }
    else
    {
        LOG_STEP("Exists: false %s", name.c_str());
        return false;
    }
}