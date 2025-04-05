#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <sys/stat.h>
#include <sstream>
#include "globals.h"
#include "directory_manager.h"
#include "constant.h"
#include "system_commands.h"
#include "process.h"

void init_system_commands() {
    status = 1;
}

int shell_help(vector<string> args) {
    if (args.size() == 1) {
        cout << "\nAVAILABLE COMMANDS:\n";
        cout << "------------------------------------------\n";
        cout << "cls              : Clear the console screen\n";
        cout << "exit             : Exit the program\n";
        cout << "pwd              : Print the current working directory\n";
        cout << "dir              : List all files and directories in the current path\n";
        cout << "cd [path]        : Change current directory or go back to parent\n";
        cout << "mkdir [name]     : Create a new directory in the current path\n";
        cout << "del [name]       : Delete a file or folder (recursively if it's a folder)\n";
        cout << "help             : Show the list of supported commands with descriptions\n";
        cout << "------------------------------------------\n";
    } else {
        cout << "Bad command...\n";
        return BAD_COMMAND;
    }
    return 0;
}

void shell_working(vector<string> args) {
    int shell_num_builtins = builtin_str.size();
    int check = 0;
    for (int i = 0; i < shell_num_builtins; i++) {
        if (args[0] == builtin_str[i]) {
            check = 1;
            (*builtin_func[i])(args);
            return;
        }
    }
    if (check == 0) {
        printf("Bad command....\n");
    }
}

string read_command_line()
{
    string line = "";
    getline(cin, line);
    return line;
}

vector<string> parse_command(string line) {
    vector<string> args;
    string arg;

    for (char ch : line) {
        if (isspace(ch)) {
            if (!arg.empty()) {
                args.push_back(arg);
                arg.clear();
            }
        } else {
            arg.push_back(ch);
        }
    }

    // Nếu sau khi duyệt hết mà vẫn còn một token chưa được thêm vào, thêm vào args
    if (!arg.empty()) {
        args.push_back(arg);
    }

    return args;
}

int shell_exit(vector<string> args) {
    if (args.size() > 1) {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    exit(EXIT_SUCCESS);
    return 0;
}

int shell_cls(vector<string> args) {
    if (args.size() > 1){
        printf("\nBad command ....\n\n");
        return 0;
    }
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return ERROR_CLS;
    }

    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    if (!FillConsoleOutputCharacter(hConsole,        // Handle đến console screen buffer
                                    (TCHAR)' ',      // Ký tự muốn ghi
                                    dwConSize,       // Số lượng ký tự cần ghi
                                    coordScreen,     // Tọa độ muốn ghi
                                    &cCharsWritten)) // Số lượng ký tự thực tế đã ghi vào console
    {
        return ERROR_CLS;
    }


    // Lấy lại thông tin màn hình
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return ERROR_CLS;
    }

    // Đặt lại các thuộc tính cho buffer
    if (!FillConsoleOutputAttribute(hConsole,         // Handle đến console screen buffer
                                    csbi.wAttributes, // Thuộc tính ký tự sử dụng
                                    dwConSize,        // Số lượng ký tự thiết lập
                                    coordScreen,      // Tọa độ
                                    &cCharsWritten))  // Số lượng ký tự thực tế
    {
        return 0;
    }

    // Đặt lại vị trí
    SetConsoleCursorPosition(hConsole, coordScreen);
    return 0;
}

int shell_time(vector<string> args) {
    if (args.size() != 1){
        printf("Bad command....\n");
        return 0;
    }
    SYSTEMTIME lt = {0};
  
    GetLocalTime(&lt);
  
    wprintf(L"\nThe local time is: %02d:%02d:%02d\n\n", 
        lt.wHour, lt.wMinute, lt.wSecond);

    return 0;
}

int shell_date(vector<string> args) {
    if (args.size() != 1){
        printf("Bad command....\n");
        return 0;
    }
    SYSTEMTIME st = {0};
  
    GetLocalTime(&st);
  
    wprintf(L"\nToday is: %d-%02d-%02d\n\n", st.wYear, st.wMonth, st.wDay);

    return 0;
}

