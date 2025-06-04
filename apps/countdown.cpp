#include <iostream>
#include <windows.h>  // Để sử dụng Sleep

int main() {
    int countdown = 100000;

    while (countdown >= 0) {
        std::cout << "Countdown: " << countdown << std::endl;
        Sleep(1000);  // Dừng 1 giây (1000 milliseconds) trước khi tiếp tục
        countdown--;
    }

    std::cout << "Countdown finished!" << std::endl;
    return 0;
}
