# Thông tin
- constant : chứa các hằng cần thiết
- globals : khai báo các biến cục bộ xuất hiện trong chương trình
- system_commands : chứa các lệnh system cần thiết như date, time, cls, exit,...
- directory_manager : chứa các lệnh quản lý về thư mục
- process : chứa các lệnh chuyên để quản lý process
- path : xử lý đường dẫn path
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

# flow_control -> luồng
# conditio_evaluator -> xử lý điều kiện luồng
# Tạo cấu trúc AST (abstract syntax tree)