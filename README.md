# Xử lý
- Cần xử lý nhiều TinyShell.exe con lồng nhau ?
- Ông để ý cái này nhé :)
- Pause theo kiểu riêng lẻ, không theo lồng nhau
# Thông tin
- constant.h : chứa các hằng cần thiết
- globals.h : khai báo các biến cục bộ xuất hiện trong chương trình
- system_commands.h : chứa các lệnh system cần thiết như date, time, cls, exit,...
- directory_manager.h : chứa các lệnh quản lý về thư mục
- process.h : chứa các lệnh chuyên để quản lý process

# Các file .bat
- build_cpp_tool.bat : build lại các chương trình con trong cpp
- move.bat : chuyển các file thực thi đã build bên trên vào /root/exe
- build.bat : build lại hệ thống
- run.bat : chạy lại hệ thống

# Thứ tự chạy (theo thứ tự):
- build_cpp_tool.bat
- move.bat
- build.bat
- run.bat


# Khi thêm thì thêm vào CMakeLists.txt (vào add_executable)
- Có cả ở thư mục gốc (TinyShell) và các chương trình con ở cpp

# Lưu ý
- Muốn test (đệ quy) liệt kê process thì phải copy từ build\\Releas\\TinyShell.exe
sang root\\exe (đối với /root/bat/test3.bat)