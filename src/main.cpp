#include <iostream>
#include <LogManager.h>
#include <FileReader.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main()
{

    std::string file = ReadFile("data/config.json");
    json data = json::parse(file);
    std::string projectVersion = data["version"].get<std::string>();
    int nested = data["nested"]["data"].get<int>();

    std::cout << nested << std::endl;
    
    return 0;
}