cmake_minimum_required(VERSION 3.10)
project(TinyShell)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include path
include_directories(include)

# Add executable and all .cpp files
add_executable(TinyShell
src/main.cpp
src/directory_manager.cpp
src/system_commands.cpp
src/utils.cpp
src/path_manager.cpp
src/process.cpp
src/cal.cpp
src/condition_evaluator.cpp
src/flow_control.cpp
src/variable_manager.cpp)
