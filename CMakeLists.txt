cmake_minimum_required(VERSION 3.30)

project(application)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(application ${SOURCES})

add_subdirectory(lib)
add_subdirectory(deps)


target_include_directories(application PUBLIC lib/LogManager lib/FileReader)
target_link_libraries(application LogManager FileReader nlohmann_json::nlohmann_json)
