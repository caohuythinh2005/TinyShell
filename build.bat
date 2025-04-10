@echo off

:: Tạo thư mục build nếu chưa có
if not exist "build" (
    mkdir build
)

:: Chuyển vào thư mục build
cd build

:: Chạy cmake để cấu hình dự án
cmake -G "Visual Studio 17 2022" -A x64 ..

:: Xây dựng dự án với cấu hình Release
cmake --build . --config Release

pause
