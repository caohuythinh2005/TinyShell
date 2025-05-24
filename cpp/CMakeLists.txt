cmake_minimum_required(VERSION 3.12)
project(TinyShell)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Thu thập tất cả các file .cpp trong thư mục
file(GLOB SOURCE_FILES "*.cpp")

# Tạo thư mục để lưu các file thực thi (exe)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

# Biên dịch các file thành các file thực thi (exe)
foreach(SOURCE_FILE ${SOURCE_FILES})
    get_filename_component(EXEC_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${EXEC_NAME} ${SOURCE_FILE})
endforeach()