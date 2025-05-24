# Bảng Tổ Hợp Phím

| Phím/Nhóm phím             | Chế độ        | Chức năng                                   |
|----------------------------|---------------|---------------------------------------------|
| `i`                        | Normal Mode   | Chuyển sang Insert Mode (chế độ nhập liệu)  |
| `Esc`                      | Insert Mode   | Thoát về Normal Mode                        |
| `Ctrl + i` (Tab)           | Insert Mode   | Thoát về Normal Mode                        |
| `:`                        | Normal Mode   | Vào Command Mode (nhập lệnh)                |
| `Enter`                    | Command Mode  | Thực thi lệnh (`:w`, `:q`, `:wq` ...)       |
| `Esc`                      | Command Mode  | Hủy nhập lệnh, trở về Normal Mode           |
| `phim trai`                | Normal Mode   | Di chuyển con trỏ sang trái                 |
| `phim duoi`                | Normal Mode   | Di chuyển con trỏ xuống dưới                |
| `phim tren`                | Normal Mode   | Di chuyển con trỏ lên trên                  |
| `phim phai`                | Normal Mode   | Di chuyển con trỏ sang phải                 |
| `Backspace`                | Insert Mode   | Xóa ký tự trước con trỏ                     |
| `Enter`                    | Insert Mode   | Xuống dòng mới                              |

---

# Các Lệnh Trong Command Mode

| Lệnh   | Chức năng                    |
|--------|------------------------------|
| `w`    | Lưu file hiện tại            |
| `q`    | Thoát trình soạn thảo        |
| `wq`   | Lưu file và thoát            |

---

# Cách Sử Dụng

Chạy chương trình với cú pháp: editor tenfile.txt



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
