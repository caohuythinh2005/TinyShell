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

cd ..

:: Di chuyển vào thư mục apps
cd apps

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

cd ..
cd ..
REM Kiểm tra nếu thư mục cpp\Release tồn tại
if exist "apps\Release" (
    REM Kiểm tra nếu thư mục root\exe chưa tồn tại thì tạo
    if not exist "root\apps" (
        mkdir "root\apps"
    )

    REM Sao chép tất cả các tệp .exe từ cpp\Release vào root\exe
    copy "apps\Release\*.exe" "root\apps\"

    echo Files copied successfully to root\exe.
) else (
    echo The source directory apps\Release does not exist.
)




pause
