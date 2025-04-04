cmake_minimum_required(VERSION 3.10)
project(TinyShell)
set(CMAKE_CXX_STANDARD 17)

# Include path
include_directories(include)

# Add executable and all .cpp files
add_executable(TinyShell
src/main.cpp
src/directory_manager.cpp
src/system_commands.cpp)