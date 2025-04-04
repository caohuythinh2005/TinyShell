// #include <windows.h>
// #include <stdio.h>

// // int main() {
// //     STARTUPINFO si;
// //     PROCESS_INFORMATION pi;

// //     ZeroMemory(&si, sizeof(si));
// //     si.cb = sizeof(si);  // Thiết lập kích thước

// //     ZeroMemory(&pi, sizeof(pi));

// //     if (CreateProcess(
// //             NULL,           // Tên file thực thi
// //             "notepad.exe",  // Lệnh chạy
// //             NULL, NULL,     // Không có bảo mật đặc biệt
// //             FALSE,          // Không kế thừa handle
// //             0,              // Không có cờ đặc biệt
// //             NULL,           // Không có biến môi trường riêng
// //             NULL,           // Sử dụng thư mục hiện tại
// //             &si,            // Thông tin khởi động
// //             &pi)) {         // Nhận thông tin tiến trình
// //         printf("Process started successfully!\n");

// //         // Chờ tiến trình kết thúc
// //         WaitForSingleObject(pi.hProcess, INFINITE);

// //         // Đóng handle
// //         CloseHandle(pi.hProcess);
// //         CloseHandle(pi.hThread);
// //     } else {
// //         printf("Failed to start process. Error: %d\n", GetLastError());
// //     }

// //     return 0;
// // }