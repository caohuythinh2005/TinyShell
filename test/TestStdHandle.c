// #include <windows.h>
// #include <stdio.h>

// int main() {
//     // HANDLE hConsole;
//     // hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

//     // // Đổi màu chữ sang xanh dương
//     // SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
//     // printf("Hello, this is blue text!\n");

//     // // Đổi màu chữ lại thành mặc định (trắng)
//     // SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
//     // printf("This is normal text.\n");

//     // return 0;

//     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//     DWORD mode;
    
//     // Lấy chế độ hiện tại của console
//     GetConsoleMode(hConsole, &mode);
    
//     // Xóa buffer bằng cách bật chế độ "ENABLE_PROCESSED_OUTPUT"
//     SetConsoleMode(hConsole, mode | ENABLE_PROCESSED_OUTPUT);
    
//     // Gửi lệnh "ESC[2J" để xóa màn hình (giống ANSI escape code)
//     printf("\x1b[2J");

//     return 0;
// }

