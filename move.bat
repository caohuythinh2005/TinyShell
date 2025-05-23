@echo off

REM Kiểm tra nếu thư mục cpp\Release tồn tại
if exist "cpp\Release" (
    REM Kiểm tra nếu thư mục root\exe chưa tồn tại thì tạo
    if not exist "root\exe" (
        mkdir "root\exe"
    )

    REM Sao chép tất cả các tệp .exe từ cpp\Release vào root\exe
    copy "cpp\Release\*.exe" "root\exe\"

    echo Files copied successfully to root\exe.
) else (
    echo The source directory cpp\Release does not exist.
)

pause
