@echo off

:: Tạo thư mục build nếu chưa có
if not exist "build" (
    mkdir build
)

:: Chuyển vào thư mục build
cd build

:: Chạy cmake để cấu hình dự án
cmake -G "MinGW Makefiles" ..

:: Xây dựng dự án với cấu hình Release
cmake --build . --config Release

pause
