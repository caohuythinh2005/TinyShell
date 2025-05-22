#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <sys/stat.h>
#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <psapi.h>
#include <sstream>
#include "path_manager.h"
#include "constant.h"
#include "utils.h"
#include "directory_manager.h"
#include <algorithm>
#include "globals.h"
#include "system_commands.h"
vector<string> split_path(const string& path) {
    vector<string> directories;
    stringstream ss(path);
    string dir;
    while (getline(ss, dir, ';')) {
        if (!dir.empty()) {
            directories.push_back(dir);
        }
    }
    return directories;
}

string replaceRoot(const string& value, const string& root_path) {
    string result = value;
    size_t pos = result.find(root_path);
    if (pos != string::npos) {
        result.replace(pos, root_path.length(), "\\root");
    }
    return result;
}

int shell_path(vector<string> args) {
    // cout << "fix_real_path is " << fixed_real_path << endl;
    // cout << "current_real_path" << current_real_path << endl;
    // cout << "origin_real_path" << origin_real_path<< endl;
    // cout << "fake_path " << current_fake_path << endl;
    if (args.size() > 1) {
        cout << "Bad Command ... \n";
        return BAD_COMMAND;
    }

    const char* path_env = getenv("PATH");
    // cout << path_env << endl;
    if (path_env == nullptr) {
        cout << "Failed to retrieve PATH environment variable.\n";
        return BAD_COMMAND;
    }

    vector<string> directories = split_path(path_env);
    bool found = false;
    bool flag = false;
    int i = 0;
    for (const string& dir : directories) {
        // cout << ++i << endl;
        // cout << "dir is : " << dir << endl;
        if (isPrefix(dir, fixed_real_path) || isPrefix(dir, current_fake_path) || isPrefix(origin_real_path + dir, fixed_real_path)) {
            string relative_path = removePrefix(dir, origin_real_path);
            // cout << "relative_path is " << relative_path << endl;
            string unix_relative_path = formatFakePathToUnixStyle(relative_path);
            // cout << "unix_relative_path " << unix_relative_path << endl;
            // string virtual_path = "\\root" + unix_relative_path;
            if(flag == false){
                cout << "PATH=";
                flag = true;
            }
            cout << unix_relative_path << ";";
            found = true;
        }
    }

    if (!found) {
        cout << "No PATH entries start with /root.";
    }
    cout << "\n";
    return 0;
}



// Cập nhật biến môi trường với giá trị mới (chỉ trong cục bộ, nếu muốn thêm vĩnh viễn thì tạo thêm hàm setx là đc)
int shell_set(vector<string> args) {
    if (args.size() == 1) {
        // Hiển thị các biến môi trường liên quan đến /root
        char** env = environ;
        if (!env) {
            cout << "Failed to retrieve environment variables.\n";
            return BAD_COMMAND;
        }

        for (char** current = env; *current; ++current) {
            string env_str = *current;
            size_t equal_pos = env_str.find('=');
            // cout << "equal_pos: " << equal_pos << endl;
            if (equal_pos != string::npos) {
                string var = env_str.substr(0, equal_pos);
                transform(var.begin(), var.end(), var.begin(), ::tolower);
                string value = env_str.substr(equal_pos + 1);
                // cout << "var: " << var << endl;
                // cout << "value " << value << endl;
                if (var == "path") {
                    // Xử lý PATH: chỉ hiển thị các đường dẫn trong /root
                    // vector<string> directories = split_path(value);
                    // string filtered_path;
                    // bool has_root_related = false;

                    // for (const string& dir : directories) {
                    //     string real_dir = dir;
                    //     // Nếu đường dẫn chứa fixed_real_path, thay bằng /root
                    //     if (isPrefix(real_dir, fixed_real_path)) {
                    //         string virtual_path = replaceRoot(real_dir, fixed_real_path);
                    //         if (!filtered_path.empty()) filtered_path += ";";
                    //         filtered_path += virtual_path;
                    //         has_root_related = true;
                    //     }
                    // }

                    // if (has_root_related) {
                    //     cout << "Path=" << filtered_path << "\n";
                    // }
                    shell_path({"path"});
                } else {
                    // Xử lý các biến khác: chỉ hiển thị nếu giá trị liên quan đến /root
                    string real_value = value;
                    // cout << "real_value is " << real_value << endl;
                    if (real_value.find("\\") != string::npos || real_value.find("/") != string::npos){
                        if (isPrefix(real_value, fixed_real_path)) {
                            string virtual_value = replaceRoot(real_value, fixed_real_path);
                            cout << var << "=" << virtual_value << "\n";
                        }
                    }
                    else{
                        cout << var << "=" << real_value << "\n";
                    }
                }
            }
        }
        return 0;
    }

    if (args.size() == 2) {
        string arg = args[1];
        transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
        size_t equal_pos = arg.find('=');
        if (equal_pos == string::npos) {
            // Hiển thị giá trị của biến
            const char* value = getenv(arg.c_str());
            if (!value) {
                cout << "Environment variable does not exist.\n";
                return BAD_COMMAND;
            }
            string val_str = value;
            if (arg == "path") {
                shell_path({"path"});
            } else {
                string real_value = val_str;
                if (real_value.find("\\") == 0 || real_value.find("/") == 0) {
                    if (isPrefix(real_value, fixed_real_path)) {
                        string virtual_value = replaceRoot(real_value, fixed_real_path);
                        cout << arg << "=" << virtual_value << "\n";
                    }
                } else {
                    cout << arg << "=" << real_value << "\n";
                }
            }
            return 0;
        }

        string var = arg.substr(0, equal_pos);
        transform(var.begin(), var.end(), var.begin(), ::tolower);
        string value = (equal_pos == arg.length() - 1) ? "" : arg.substr(equal_pos + 1);

        if (value.empty()) {
            // Xóa biến
            string unset_cmd = var + "=";
            if (_putenv(unset_cmd.c_str()) != 0) {
                cout << "Failed to delete environment variable.\n";
                return BAD_COMMAND;
            }
            cout << "Environment variable " << var << " deleted.\n";
        } else {
            // Thiết lập biến
            string set_cmd = arg;
            if (_putenv(set_cmd.c_str()) != 0) {
                cout << "Failed to set environment variable.\n";
                return BAD_COMMAND;
            }
            // Kiểm tra PATH
            if (var == "path") {
                vector<string> directories = split_path(value);
                for (const string& dir : directories) {
                    string real_dir = convertFakeToRealPath(dir);
                    if (!isPrefix(real_dir, fixed_real_path)) {
                        cout << "Warning: PATH contains directory outside /root: " << dir << "\n";
                    }
                }
            }
        }
        return 0;
    }

    cout << "Usage: set [variable] [variable=value]\n";
    return BAD_COMMAND;
}

bool path_exists_and_is_directory(const string& path) {
    DWORD attrs = GetFileAttributes(path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

int shell_addpath(vector<string> args) {
    if (args.size() != 2) {
        cout << "Usage: addpath <path>\n";
        return BAD_COMMAND;
    }

    string new_path = args[1];
    cout << "new path is " << new_path << endl;
    string real_new_path = convertFakeToRealPath(new_path);
    cout << "real_new_path is " << real_new_path << endl;
    // Kiểm tra xem đường dẫn có tồn tại và là thư mục không
    if (!path_exists_and_is_directory(real_new_path)) {
        cout << "Invalid or non-existent path: " << new_path << "\n";
        return BAD_COMMAND;
    }

    // Kiểm tra xem đường dẫn có nằm trong /root không
    if (!isPrefix(real_new_path, fixed_real_path)) {
        cout << "Path must be inside /root.\n";
        return BAD_COMMAND;
    }

    // Lấy PATH hiện tại
    const char* current_path = getenv("PATH");
    string current_path_str = current_path ? current_path : "";

    // Chuyển đổi đường dẫn thực tế thành đường dẫn ảo
    string virtual_new_path = removePrefix(real_new_path, origin_real_path);

    // Kiểm tra xem đường dẫn đã có trong PATH chưa
    if (current_path_str.find(virtual_new_path) != string::npos) {
        cout << "Path " << virtual_new_path << " already exists in PATH.\n";
        return 0;
    }
    // Thêm đường dẫn ảo mới vào PATH
    string updated_path = current_path_str.empty() ? virtual_new_path : current_path_str + ";" + virtual_new_path;
    string set_cmd = "PATH=" + updated_path;
    if (_putenv(set_cmd.c_str()) != 0) {
        cout << "Failed to update PATH environment variable.\n";
        return BAD_COMMAND;
    }

    cout << "Added " << virtual_new_path << " to PATH.\n";
    return 0;
}

int shell_delpath(vector<string> args){
    if (args.size() != 2) {
        cout << "Usage: deletepath <path>\n";
        return BAD_COMMAND;
    }

    string path_to_remove = args[1];
    cout << "Path to remove is " << path_to_remove << endl;
    string real_path_to_remove = convertFakeToRealPath(path_to_remove);
    cout << "real_path_to_remove is " << real_path_to_remove << endl;

    // Kiểm tra xem đường dẫn có tồn tại và là thư mục không
    if (!path_exists_and_is_directory(real_path_to_remove)) {
        cout << "Invalid or non-existent path: " << path_to_remove << "\n";
        return BAD_COMMAND;
    }

    // Kiểm tra xem đường dẫn có nằm trong /root không
    if (!isPrefix(real_path_to_remove, fixed_real_path)) {
        cout << "Path must be inside /root.\n";
        return BAD_COMMAND;
    }

    // Lấy PATH hiện tại
    const char* current_path = getenv("PATH");
    string current_path_str = current_path ? current_path : "";
    if (current_path_str.empty()) {
        cout << "PATH is empty, nothing to delete.\n";
        return 0;
    }

    // Chuyển đổi đường dẫn thực tế thành đường dẫn ảo
    string virtual_path_to_remove = removePrefix(real_path_to_remove, origin_real_path);

    // Kiểm tra xem đường dẫn có trong PATH không
    size_t pos = current_path_str.find(virtual_path_to_remove);
    if (pos == string::npos) {
        cout << "Path " << virtual_path_to_remove << " does not exist in PATH.\n";
        return 0;
    }

    // Xóa đường dẫn khỏi PATH
    string updated_path = current_path_str;
    if (pos > 0 && updated_path[pos - 1] == ';') {
        // Xóa cả dấu chấm phẩy phía trước nếu có
        updated_path.erase(pos - 1, virtual_path_to_remove.length() + 1);
    } else if (pos == 0 && updated_path.length() > virtual_path_to_remove.length() && updated_path[virtual_path_to_remove.length()] == ';') {
        // Xóa dấu chấm phẩy phía sau nếu có
        updated_path.erase(pos, virtual_path_to_remove.length() + 1);
    } else {
        // Xóa chính xác đường dẫn
        updated_path.erase(pos, virtual_path_to_remove.length());
    }

    // Nếu PATH rỗng sau khi xóa, đặt lại thành chuỗi rỗng
    if (updated_path.empty()) {
        updated_path = "";
    }

    // Cập nhật PATH
    string set_cmd = "PATH=" + updated_path;
    if (_putenv(set_cmd.c_str()) != 0) {
        cout << "Failed to update PATH environment variable.\n";
        return BAD_COMMAND;
    }

    cout << "Deleted " << virtual_path_to_remove << " from PATH.\n";
    return 0;
}


void sigintHandler(int sig_num) {
    // Nếu có tiến trình foreground đang chạy, terminate nó
    if (fore != NULL) {
        cout << "\nInterrupt received. Terminating foreground process...\n";
        TerminateProcess(fore, 1); // Dừng tiến trình
        fore = NULL;  // Xóa handle
    }
    cin.clear(); // Đảm bảo không bị lỗi khi đọc từ stdin
}


// Danh sách lưu thông tin các tiến trình con
vector<PROCESS_INFORMATION> childProcesses;



string expandEnvVars(const string& path) {
    char expanded[MAX_PATH];
    DWORD result = ExpandEnvironmentStringsA(path.c_str(), expanded, MAX_PATH);
    if (result == 0 || result > MAX_PATH) {
        return path;
    }
    return string(expanded);
}

// Hàm tạm dừng tiến trình con theo PID
void pauseChild(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pid);
    if (hProcess) {
        SuspendThread(hProcess); // Tạm dừng tiến trình
        CloseHandle(hProcess);
        printf("Process %lu paused.\n", pid);
    } else {
        printf("Failed to pause process %lu. Error: %lu\n", pid, GetLastError());
    }
}

// Hàm tiếp tục tiến trình con theo PID
void resumeChild(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pid);
    if (hProcess) {
        ResumeThread(hProcess); // Tiếp tục tiến trình
        CloseHandle(hProcess);
        printf("Process %lu resumed.\n", pid);
    } else {
        printf("Failed to resume process %lu. Error: %lu\n", pid, GetLastError());
    }
}

int shell_runExe(vector<string> args) {
        if (args.size() < 2 || args.size() > 4) {
            printf("Usage: runExe [path_or_name] [-b] [-c]\n");
            return BAD_COMMAND;
        }
    
        string input = args[1];
        string realPath;
        string rootPath = origin_real_path + "\\root\\.."; // current_real_path
        bool isPath = (input.find("\\") == 0 || input.find("/") == 0); // Kiểm tra xem có phải đường dẫn không
        // cout << "isPath: " << isPath << endl;
        // cout << " current_real_path: " << current_real_path << endl;
        // cout << "rootpath is: " << rootPath<< endl;
        DWORD file1 = GetFileAttributesA((current_real_path + "\\" + input).c_str());
        DWORD file2 = GetFileAttributesA((current_real_path + "\\" + input + ".exe").c_str());
        if(file1 != INVALID_FILE_ATTRIBUTES && !(file1 & FILE_ATTRIBUTE_DIRECTORY)){
            // cout <<"file1 is " << file1 << endl;
            realPath = current_real_path + "\\" + input;
            // cout << "real path is: " << realPath << endl;
            // cout << "find: " << realPath.find(rootPath) << endl;
            if(realPath.find(rootPath) == 0){
                printf("Error: not found: ", input.c_str());
                return BAD_COMMAND;
            }
        }
        else if(file2 != INVALID_FILE_ATTRIBUTES && !(file2 & FILE_ATTRIBUTE_DIRECTORY)) {
            // cout << "file 2 is: " << file2 << endl;
            realPath = current_real_path + "\\" + input + ".exe";
            // cout << "real path is: " << realPath << endl;
            if(realPath.find(rootPath) == 0){
                printf("Error: not found: ", input.c_str());
                return BAD_COMMAND;
            }
        }
        else{
            if (isPath) {
                realPath = convertFakeToRealPath(input);
                // cout << "realPath in isPath is: " << realPath << endl;
            } 
            else {
                const char* path_env = getenv("PATH");
                if (!path_env) {
                    printf("PATH environment variable not set.\n");
                    return BAD_COMMAND;
                }
                // cout << "input: " << input << endl;
                vector<string> directories = split_path(path_env);
                bool found = false;
                for (const string& dir : directories) {
                    // cout << "dir: " << dir << endl;
                    string testPath = origin_real_path + dir + "\\" + input;
                    // cout << "testPath :" << testPath << endl;
                    if (dir.find("\\root") == 0) { // Nếu là đường dẫn ảo trong Tiny Shell
                        testPath = convertFakeToRealPath(dir) + "\\" + input;
                    }
                    // Kiểm tra file .exe
                    string testPathWithExe = testPath + ".exe";
                    DWORD fileAttr = GetFileAttributesA(testPathWithExe.c_str());
                    if (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                        realPath = testPathWithExe;
                        found = true;
                        break;
                    }
                    // Kiểm tra file .bat
                    string testPathWithBat = testPath + ".bat";
                    fileAttr = GetFileAttributesA(testPathWithBat.c_str());
                    if (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                        realPath = testPathWithBat;
                        found = true;
                        break;
                    }
                    // // Thêm đuôi .exe
                    // if (testPath.find(".exe") == string::npos) {
                    //     testPath += ".exe";
                    // }
                    fileAttr = GetFileAttributesA(testPath.c_str());
                    if (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                        realPath = testPath;
                        found = true;
                        break;
                    }
                    // printf("Checking path: %s\n", testPath.c_str());
                }
        
                if (!found) {
                    printf("Executable '%s' not found in PATH.\n", input.c_str());
                    return BAD_COMMAND;
                }
            }
        }
    printf("Attempting to execute: %s\n", realPath.c_str());
    DWORD fileAttr = GetFileAttributesA(realPath.c_str());
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        printf("File does not exist: %s\n", realPath.c_str());
        return BAD_COMMAND;
    }
    if (fileAttr & FILE_ATTRIBUTE_DIRECTORY) {
        printf("Path is a directory, not a file: %s\n", realPath.c_str());
        return BAD_COMMAND;
    }

    bool isExecutable = (realPath.find(".exe") != string::npos);
    // bool isfile = (realPath.find(".bat") != string::npos);
    bool isfilebat = (realPath.find(".bat") != string::npos);
    int isfile = fileExists(input);
    bool isBackground = false;
    bool createConsole = false;

    for (int i = 2; i < args.size(); ++i) {
        if (args[i] == "-b") isBackground = true;
        else if (args[i] == "-c") createConsole = true;
        else {
            printf("Unknown flag: %s\n", args[i].c_str());
            return BAD_COMMAND;
        }
    }

    // cout << "isExecutable : " << isExecutable << endl;
    // cout << "isfilebat: " << isfilebat << endl;
    // cout << "isfile: " << isfile << endl;
    if (isExecutable) {
        string cmdLine = (realPath.find(".bat") != string::npos) ? "cmd.exe /c \"" + realPath + "\"" : realPath;
        char* cmdLineBuffer = new char[cmdLine.length() + 1];
        strcpy(cmdLineBuffer, cmdLine.c_str());

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        DWORD creationFlags = CREATE_UNICODE_ENVIRONMENT;

        HANDLE hNull = NULL;
        if (isBackground && !createConsole) {
            hNull = CreateFileA("NUL", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hNull != INVALID_HANDLE_VALUE) {
                si.dwFlags |= STARTF_USESTDHANDLES;
                si.hStdOutput = hNull;
                si.hStdError = hNull;
            }
        }

        if (createConsole) {
            creationFlags |= CREATE_NEW_CONSOLE;
        }

        BOOL success = CreateProcessA(
            NULL,
            cmdLineBuffer,
            NULL,
            NULL,
            TRUE,
            creationFlags,
            NULL,
            NULL,
            &si,
            &pi
        );

        if (hNull) CloseHandle(hNull);
        delete[] cmdLineBuffer;

        if (!success) {
            DWORD errorCode = GetLastError();
            cerr << "Failed to create process. Error code: " << errorCode << "\n";
            if (errorCode == ERROR_FILE_NOT_FOUND) {
                cerr << "Reason: The system cannot find the file specified.\n";
            }
            return 1;
        }

        // Lưu thông tin tiến trình con
        childProcesses.push_back(pi);
        printf("Child process created with PID: %lu\n", pi.dwProcessId);

        // Đăng ký xử lý tín hiệu cho tiến trình hiện tại
        signal(SIGINT, sigintHandler);

        if (!isBackground) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            printf("Child Complete\n");
            // Xóa tiến trình khỏi danh sách khi hoàn thành
            childProcesses.erase(childProcesses.end() - 1);
        }
        // Không đóng handle ngay nếu chạy nền, để quản lý sau
    } else if (isfilebat) {
        // cout << "test " << endl;
        ifstream scriptFile(realPath);
        if (!scriptFile)
        {
            cout << "UNABLE TO OPEN SCRIPT FILE: " << realPath << endl;
            return UNABLE_TO_OPEN_SCRIPT_FILE;
        }
        string line;
        while (getline(scriptFile, line))
        {
            args = parse_command(line);
            if (args.size() > 0) {
                shell_working(args);
            }
        }

        scriptFile.close();
        return 0;
    }
     else {
        HINSTANCE result = ShellExecuteA(NULL, "open", realPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
        if ((intptr_t)result <= 32) {
            cerr << "Failed to open file. Error code: " << (intptr_t)result << "\n";
            return 1;
        }
        if (!isBackground) {
            printf("File opened.\n");
        }
    }

    return 0;
}

// // Hàm mới để tạm dừng tiến trình con theo PID
// int shell_pause(vector<string> args) {
//     if (args.size() != 2) {
//         printf("Usage: pause <pid>\n");
//         return BAD_COMMAND;
//     }

//     DWORD pid = atoi(args[1].c_str());
//     pauseChild(pid);
//     return 0;
// }

// // Hàm mới để tiếp tục tiến trình con theo PID
// int shell_resume(vector<string> args) {
//     if (args.size() != 2) {
//         printf("Usage: resume <pid>\n");
//         return BAD_COMMAND;
//     }

//     DWORD pid = atoi(args[1].c_str());
//     resumeChild(pid);
//     return 0;
// }

int shell_where(vector<string> args){
    if(args.size() != 2){
        cout << "Bad command... \n";
        return BAD_COMMAND;
    }
    string input = args[1];

    const char* path_env = getenv("PATH");
    // cout << path_env << endl;
    if (path_env == nullptr) {
        cout << "Failed to retrieve PATH environment variable.\n";
        return BAD_COMMAND;
    }

    vector<string> directories = split_path(path_env);
    vector<string> path_tiny_shell;
    bool found = false;

    for (const string& dir : directories) {
        // cout << ++i << endl;
        // cout << "dir is : " << dir << endl;
        if (isPrefix(dir, fixed_real_path) || isPrefix(dir, current_fake_path) || isPrefix(origin_real_path + dir, fixed_real_path)) {
            string relative_path = removePrefix(dir, origin_real_path);
            // cout << "relative_path is " << relative_path << endl;
            string unix_relative_path = formatFakePathToUnixStyle(relative_path);
            path_tiny_shell.push_back(unix_relative_path);
        }
    }

    for(const string& dir : path_tiny_shell){
        string test_path = origin_real_path + dir + "\\" + input;
        vector<string> ex = {"", ".exe", ".bat"};
        for (const string& v : ex){
            string fullPath = test_path + v;
            DWORD fileAttr = GetFileAttributesA(fullPath.c_str());
            if(fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)){
                cout << dir + "\\" + input + v << "\n";
                found = true;
            }
        }
    }
    if(found == false){
        cout << "INFO: Could not find files for the given pattern(s).\n";
    }
    return 0;
}