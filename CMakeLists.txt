cmake_minimum_required(VERSION 3.12)
project(TinyShell)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-D_HAS_STD_BYTE=0)

include_directories(include)


# Add executable and all .cpp files using glob
file(GLOB TINYSHELL_SOURCES
    "src/*.cpp"
    "src/**/*.cpp"
    "src/**/**/*.cpp"
)

add_executable(TinyShell ${TINYSHELL_SOURCES})

target_link_libraries(TinyShell Shlwapi)