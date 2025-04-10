#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <string>
#include <filesystem>
#include <windows.h>

using namespace std;
namespace fs = std::filesystem;

// Hàm để lấy thư mục hiện tại
string getCurrentDirectory() {
    return fs::current_path().string();
}

int main() {
    // Lấy đường dẫn thư mục hiện tại
    string currentDir = getCurrentDirectory();

    // Tạo thư mục "a" nếu chưa tồn tại
    string targetDir = currentDir + "/root/exe/a";
    if (!fs::exists(targetDir)) {
        fs::create_directory(targetDir);
    }

    int i = 1;
    while (i <= 100000) {
        // Tạo tên file với số thứ tự trong thư mục "a"
        string filename = targetDir + "/" + to_string(i) + ".txt";

        // Mở file và ghi nội dung vào file
        ofstream file(filename);
        if (file.is_open()) {
            file << "This is file number " << i << endl;
            file.close();
        } else {
            cout << "Failed to create file " << filename << endl;
            break;
        }

        cout << "Created file: " << filename << endl;

        // Chờ 1 giây trước khi tạo file tiếp theo
        Sleep(1000);
        i++;
    }

    cout << "File creation process finished." << endl;
    return 0;
}
