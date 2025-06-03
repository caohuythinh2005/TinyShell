# TinyShell

TinyShell là một chương trình mô phỏng terminal đơn giản được viết bằng C++, hỗ trợ nhiều lệnh quản lý file, thư mục, biến môi trường và tiến trình. Đây là công cụ tuyệt vời để hiểu rõ hơn về cách hoạt động của shell, cũng như luyện tập kỹ năng xử lý hệ thống và lập trình hệ điều hành.

---

### 🛡️ Tính năng Sandbox và Bảo mật

Shell được thiết kế để đảm bảo tính an toàn và ổn định cho hệ thống của bạn. Mọi thao tác quản lý file và thư mục, cũng như các tiến trình được khởi chạy, **chỉ có tác dụng trong thư mục gốc (root directory) ảo của Shell**. Điều này ngăn chặn việc các lệnh vô tình hoặc cố ý gây ảnh hưởng đến các phần khác của hệ thống, bảo vệ dữ liệu quan trọng và đảm bảo môi trường làm việc luôn được kiểm soát. Bạn có thể thoải mái thử nghiệm các lệnh mà không lo lắng về việc làm hỏng cấu trúc file hệ thống thực tế.

## Thành viên
| Name                  | Student ID  |
|-----------------------|-------------|
| Cao Huy Thịnh         | 20230069    |
| Nguyễn Đình Tuấn Minh | 20230048    |

## 📌 Yêu cầu hệ thống

- **Hệ điều hành**: Windows 11
- **Trình biên dịch**: Visual Studio 2019 trở lên (có hỗ trợ C++17)
- **CMake**: phiên bản ≥ 3.12
- **Command Prompt / PowerShell**

---


## 🚀 Tính năng nổi bật

### 📁 Quản lý thư mục
| Lệnh                          | Mô tả                                                       |
|------------------------------|-------------------------------------------------------------|
| `cd [path]`                  | Di chuyển đến thư mục chỉ định                              |
| `cd ..`                      | Di chuyển về thư mục cha                                    |
| `pwd`                        | In ra thư mục hiện tại                                      |
| `dir`                        | Liệt kê tất cả file và thư mục trong thư mục hiện tại       |

### 📄 Quản lý file và thư mục
| Lệnh                          | Mô tả                                                       |
|------------------------------|-------------------------------------------------------------|
| `mkdir [name]`               | Tạo thư mục mới                                             |
| `del [name]`                 | Xóa file/thư mục (đệ quy nếu là thư mục)                   |
| `touch [-f] [name]`          | Tạo file rỗng (ghi đè nếu có `-f`)                         |
| `cat [name]`                 | Hiển thị nội dung file                                      |
| `write [-f] [-a] [name]`     | Ghi vào file (`-f`: ghi đè, `-a`: ghi thêm)                |
| `rename [old] [new]`         | Đổi tên file/thư mục                                        |
| `move [src] [dest]`          | Di chuyển file/thư mục                                      |
| `copy [src] [dest]`          | Sao chép file/thư mục                                       |

### ⚙️ Quản lý tiến trình
| Lệnh                          | Mô tả                                                       |
|------------------------------|-------------------------------------------------------------|
| `exec [path] [-b] [-c]`      | Chạy file .exe hoặc .bat; (-b: chạy nền, -c: mở cửa sổ mới) cho file .exe |
| `kill_id [pid]`              | Kết thúc tiến trình theo PID (do TinyShell quản lý)         |
| `pause_id [pid]`             | Tạm dừng tiến trình                                         |
| `resume_id [pid]`            | Tiếp tục tiến trình đã tạm dừng                             |
| `list`                       | Liệt kê các tiến trình TinyShell đang quản lý               |


### 🌐 Biến môi trường
| Lệnh                          | Mô tả                                                       |
|------------------------------|-------------------------------------------------------------|
| `set`                        | Hiển thị tất cả biến môi trường                            |
| `set VAR=value`              | Tạo hoặc cập nhật biến môi trường                           |
| `set VAR=`                   | Xóa biến môi trường                                         |
| `set /a expression`          | Tính biểu thức số học (giống CMD)                          |
| `setx VAR=value`             | Tạo biến môi trường vĩnh viễn                  |
| `path`                       | Hiển thị biến PATH                                          |
| `addpath <path>`             | Thêm thư mục vào PATH (session only)                            |
| `delpath <path>`             | Xóa thư mục khỏi PATH (session only)                |
| `addpathx <path>`            | Thêm thư mục vào PATH (permanent)                   |
| `delpathx <path>`            | Xóa thư mục khỏi PATH (permanent)                                    |
| `where`                      | Hiển thị đường dẫn đầy đủ của file `.bat` hoặc `.exe`       |
| `in name`                    | Nhập biến       |

### 🧰 Công cụ và tiện ích khác
| Lệnh                          | Mô tả                                                       |
|------------------------------|-------------------------------------------------------------|
| `time`                       | Hiển thị giờ hệ thống hiện tại                             |
| `date`                       | Hiển thị ngày hiện tại                                      |
| `test [-f] [-d] [path]`      | Kiểm tra tồn tại file (`-f`) hoặc thư mục (`-d`)             |
| `cal [num1] [op] [num2]`     | Máy tính đơn giản (`+ - * / %`)                              |
| `cls`                        | Xóa màn hình                                                |
| `exit`                       | Thoát khỏi shell                                            |
| `help`                       | Hiển thị danh sách lệnh                                     |
| `phím tab`                  | tự động hoàn thành lệnh                                     |
| `tree [đường_dẫn_tùy_chọn]`  | Hiển thị cây thư mục từ thư mục hiện tại hoặc đường dẫn cho trước |
| `color [-s,-c,-h]`           | Thay đổi màu shell (-s) hoặc màu lệnh (-c); -h xem hướng dẫn |
| `name [tên_mới]`             | Thay đổi tên của shell hiển thị ở prompt                    |

### 🔁 Cấu trúc điều khiển  
| Lệnh                                      | Mô tả                                                               |
|-------------------------------------------|---------------------------------------------------------------------|
| `if <cond>`                               | Thực thi khối lệnh nếu điều kiện đúng                              |
| `if <cond> else`                          | Thực thi khối `else` nếu điều kiện sai                             |
| `while <cond>`                            | Lặp lại khối lệnh khi điều kiện đúng                               |
| `for (<var>=<start>;<cond>;<update>)`     | Vòng lặp kiểu C: khởi tạo, điều kiện và cập nhật                   |
| `eval <expression>`                       | Đánh giá biểu thức logic hoặc số học                               |


### 📝 Trình soạn thảo tích hợp
| Lệnh                          | Mô tả                                                       |
|------------------------------|-------------------------------------------------------------|
| `editor -i <file>`           | Mở file để chỉnh sửa tương tác                             |
| `editor -h`                  | Hiển thị hướng dẫn phím tắt                                 |

---

## ⚙️ Hướng dẫn cài đặt và chạy

### ✅ Cài đặt nhanh

1. **Clone dự án**:

    ```bash
    git clone https://github.com/caohuythinh2005/TinyShell.git
    cd tinyshell
    ```

2. **Build với `build.bat` (dành cho Windows + Visual Studio)**:

    ```bash
    build.bat
    ```

3. **Chạy TinyShell với `run.bat`**:

    ```bash
    run.bat
    ```

---
