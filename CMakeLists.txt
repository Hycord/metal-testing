cmake_minimum_required(VERSION 3.30)
project(application)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")

# Collect source files
file(GLOB_RECURSE SOURCES src/*.cpp src/*.mm)

# Create executable
add_executable(application ${SOURCES})

# Add dependencies
add_subdirectory(deps)

# Find macOS frameworks
find_library(METAL Metal)
find_library(FOUNDATION Foundation)
find_library(QUARTZCORE QuartzCore)
find_library(APPKIT AppKit)
find_library(METALKIT MetalKit)

# Include directories
target_include_directories(application PRIVATE 
    deps/Apple
    deps/stb
    src
)

# Link libraries
target_link_libraries(application 
    PRIVATE
    "-framework Metal"
    "-framework Foundation" 
    "-framework QuartzCore"
    "-framework AppKit"
    "-framework MetalKit"
    nlohmann_json::nlohmann_json
    objc
    glfw
)

# # Set Objective-C++ linking flags
# set_target_properties(application PROPERTIES 
#     LINK_FLAGS "-ObjC"
#     MACOSX_BUNDLE TRUE  # Creates .app bundle
# )
