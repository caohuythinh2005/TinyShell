cmake_minimum_required(VERSION 3.12)
project(TinyShell)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-D_HAS_STD_BYTE=0)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

if(MSVC)
    # Tắt cảnh báo C4700 (uninitialized variable)
    add_compile_options(/wd4700)
    
    # Tắt cảnh báo C4715 (not all control paths return a value)
    add_compile_options(/wd4715)

    # Tắt cảnh báo C4267 (conversion from 'size_t' to 'int')
    add_compile_options(/wd4267)

    # Tắt cảnh báo C4101 (unreferenced local variable)
    add_compile_options(/wd4101)

    # Tắt cảnh báo C4477 (printf format mismatch)
    add_compile_options(/wd4477)
endif()

include_directories(include)

file(GLOB TINYSHELL_SOURCES
    "src/*.cpp"
    "src/**/*.cpp"
    "src/**/**/*.cpp"
)

add_executable(TinyShell ${TINYSHELL_SOURCES})

target_link_libraries(TinyShell Shlwapi)
