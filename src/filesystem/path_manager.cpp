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
#include "filesystem/path_manager.h"
#include "utilities/constant.h"
#include "utilities/utils.h"
#include "filesystem/directory_manager.h"
#include <algorithm>
#include "variables/variable_manager.h"
#include "utilities/constant.h"
#include "parsing/ast/builder.h"
#include "parsing/script_io.h"
#include "utilities/globals.h"
#include "execution/system_commands.h"
vector<string> envPaths;
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

    // const char* path_env = getenv("PATH");
    // // cout << path_env << endl;
    // if (path_env == nullptr) {
    //     cout << "Failed to retrieve PATH environment variable.\n";
    //     return BAD_COMMAND;
    // }
    if(envPaths.empty()){
        cout << "No PATH entries start with /root.\n";
        return 0;
    }
    // vector<string> directories = split_path(path_env);
    bool found = false;
    bool flag = false;
    int i = 0;
    for (const string& dir : envPaths) {
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

bool path_exists_and_is_directory(const string& path) {
    DWORD attrs = GetFileAttributes(path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

// int shell_addpath(vector<string> args) {
//     if (args.size() != 2) {
//         cout << "Usage: addpath <path>\n";
//         return BAD_COMMAND;
//     }

//     string new_path = args[1];
//     cout << "new path is " << new_path << endl;
//     string real_new_path = getNormalizedDirectory(new_path);
//     cout << "real_new_path is " << real_new_path << endl;
//     // Kiểm tra xem đường dẫn có tồn tại và là thư mục không
//     if (!path_exists_and_is_directory(real_new_path)) {
//         cout << "Invalid or non-existent path: " << new_path << "\n";
//         return BAD_COMMAND;
//     }

//     // Kiểm tra xem đường dẫn có nằm trong /root không
//     if (!isPrefix(real_new_path, fixed_real_path) || folderExists(new_path) != 0) {
//         cout << "Path must be inside /root.\n";
//         return BAD_COMMAND;
//     }

//     // Lấy PATH hiện tại
//     const char* current_path = getenv("PATH");
//     string current_path_str = current_path ? current_path : "";
//     cout << "current_path_str: " << current_path_str << "\n";
//     // Chuyển đổi đường dẫn thực tế thành đường dẫn ảo
//     string virtual_new_path = removePrefix(real_new_path, origin_real_path);
//     // cout << "virtual_new+path = " << virtual_new_path << "\n";
//     // Kiểm tra xem đường dẫn đã có trong PATH chưa
//     if (current_path_str.find(virtual_new_path) != string::npos) {
//         cout << "Path " << virtual_new_path << " already exists in PATH.\n";
//         return 0;
//     }
//     // Thêm đường dẫn ảo mới vào PATH
//     string updated_path = current_path_str.empty() ? virtual_new_path : current_path_str + ";" + virtual_new_path;
//     string set_cmd = "PATH=" + updated_path;
//     if (_putenv(set_cmd.c_str()) != 0) {
//         cout << "Failed to update PATH environment variable.\n";
//         return BAD_COMMAND;
//     }

//     cout << "Added " << virtual_new_path << " to PATH.\n";
//     return 0;
// }

void initPath() {
    auto it = session_vars.find("PATH");
    if (it == session_vars.end()) {
        cout << "No PATH variable found in session variables.\n";
        return;
    }

    string path_value = it->second;
    if (path_value.empty()) {
        cout << "PATH is empty.\n";
        return;
    }

    string current_path = "";
    vector<string> paths = split_path(path_value);
    paths = formatToRealPath(paths);
    // for(auto p : paths){
    //     cout << p << "\n";
    // }
    for (string& virtual_path : paths) {
        string realPath = getNormalizedDirectory(virtual_path);

        // Kiểm tra thư mục tồn tại thực sự không
        if (!path_exists_and_is_directory(realPath)) {
            // cout << "Invalid path: " << virtual_path << "\n";
            continue;
        }

        // Kiểm tra xem có nằm trong /root không
        if (folderExists(virtual_path) != 0) {
            cout << "Path must be inside /root.\n";
            continue;
        }

        current_path = current_path.empty() ? virtual_path : current_path + ";" + virtual_path;
        envPaths.push_back(virtual_path);
    }

    // if (envPaths.empty()) {
    //     cout << "No valid paths found in PATH.\n";
    // }
}


bool isPathInEnvList(const string& path, const vector<string>& envPaths) {
    string normalized_path = getNormalizedDirectory(path);
    for (const string& envPath : envPaths) {
        if (getNormalizedDirectory(envPath) == normalized_path) {
            return true;
        }
    }
    return false;
}
// Add trong phiên làm việc của tiny shell (Không phải thêm toàn cục)
int shell_addpath(vector<string> args){
    // static bool initialized = false;
    // if (!initialized) {
    //     initPath();
    //     initialized = true;
    // }
    if(args.size() != 2){
        cout << "Usage: addpath <path>\n";
        return BAD_COMMAND;
    }
    string new_path = args[1];
    string real_new_path = getNormalizedDirectory(new_path);
    // cout << "real_new_path: " << real_new_path << "\n";
    if (!path_exists_and_is_directory(real_new_path)) {
        cout << "Invalid or non-existent path: " << formatFakePathToUnixStyle(new_path) << "\n";
        return BAD_COMMAND;
    }
    if(folderExists(new_path) != 0){
        cout << "Path must be inside /root.\n";
        return ERROR_PATH;
    }
    string virtual_new_path = convertRealToFakePath(real_new_path);
    // if (isPathInEnvList(virtual_new_path, envPaths)) {
    //     cout << "Path " << virtual_new_path << " already exists in myshell.\n";
    //     return ERROR_PATH;
    // }
    envPaths.push_back(virtual_new_path);
    cout << "Added: " << formatFakePathToUnixStyle(virtual_new_path) << " to PATH \n";
    return 0;
}


int shell_addpathx(vector<string> args) {
    if (args.size() != 2) {
        cout << "Usage: addpathx <path>\n";
        return BAD_COMMAND;
    }
    string new_path = args[1];
    string real_new_path = getNormalizedDirectory(new_path);
    if (!path_exists_and_is_directory(real_new_path)) {
        cout << "Invalid or non-existent path: " << formatFakePathToUnixStyle(new_path) << "\n";
        return BAD_COMMAND;
    }
    string virtual_new_path = convertRealToFakePath(real_new_path);
    if (folderExists(new_path) != 0) {
        cout << "Path must be inside /root.\n";
        return ERROR_PATH;
    }
    // if (isPathInEnvList(virtual_new_path, envPaths)) {
    //     cout << "Path " << virtual_new_path << " already exists in PATH.\n";
    //     return ERROR_PATH;
    // }

    envPaths.push_back(virtual_new_path);
    
    persistent_vars["PATH"] = persistent_vars["PATH"] + ";" + virtual_new_path;
    session_vars["PATH"] = session_vars["PATH"] + ";" + virtual_new_path;
    save_persistent_vars();

    cout << "Added: " << formatFakePathToUnixStyle(virtual_new_path) << " to PATH \n";
    return 0;
}

int shell_delpath(vector<string> args){
    if (args.size() != 2) {
        cout << "Usage: deletepath <path>\n";
        return BAD_COMMAND;
    }

    string path_to_remove = args[1];
    // cout << "Path to remove is " << path_to_remove << endl;
    string real_path_to_remove = getNormalizedDirectory(path_to_remove);
    // cout << "real_path_to_remove is " << real_path_to_remove << endl;
    // Kiểm tra xem đường dẫn có tồn tại và là thư mục không
    if (!path_exists_and_is_directory(real_path_to_remove)) {
        cout << "Invalid or non-existent path: " << formatFakePathToUnixStyle(path_to_remove) << "\n";
        return BAD_COMMAND;
    }

    // Kiểm tra xem đường dẫn có nằm trong /root không
    if (folderExists(path_to_remove) != 0) {
        cout << "Path must be inside /root.\n";
        return BAD_COMMAND;
    }

    string path_remove = convertRealToFakePath(real_path_to_remove);
     auto it = find(envPaths.begin(), envPaths.end(), path_remove);

    // Kiểm tra nếu tìm thấy
    if (it != envPaths.end()) {
        envPaths.erase(remove(envPaths.begin(), envPaths.end(), path_remove), envPaths.end());
    } else {
        cout << "Can not remove path. \n";
    }
    return 0;
}

int shell_delpathx(vector<string> args) {
    if (args.size() != 2) {
        cout << "Usage: delpathx <path>\n";
        return BAD_COMMAND;
    }

    string path_to_remove = args[1];
    string real_path_to_remove = getNormalizedDirectory(path_to_remove);

    if (!path_exists_and_is_directory(real_path_to_remove)) {
        cout << "Invalid or non-existent path: " << formatFakePathToUnixStyle(path_to_remove) << "\n";
        return BAD_COMMAND;
    }

    if (folderExists(path_to_remove) != 0) {
        cout << "Path must be inside /root.\n";
        return BAD_COMMAND;
    }

    string path_remove = convertRealToFakePath(real_path_to_remove);

    auto it = find(envPaths.begin(), envPaths.end(), path_remove);
    if (it != envPaths.end()) {
        envPaths.erase(remove(envPaths.begin(), envPaths.end(), path_remove), envPaths.end());
    } else {
        cout << "Can not remove path.\n";
    }

    string& persistentPath = persistent_vars["PATH"];
    vector<string> paths;
    stringstream ss(persistentPath);
    string segment;
    while (getline(ss, segment, ';')) {
        if (segment != path_remove) {
            paths.push_back(segment);
        }
    }
    ostringstream oss;
    for (size_t i = 0; i < paths.size(); ++i) {
        oss << paths[i];
        if (i != paths.size() - 1) oss << ";";
    }
    persistent_vars["PATH"] = oss.str();

    session_vars["PATH"] = persistent_vars["PATH"];

    save_persistent_vars();

    cout << "Removed permanently: " << formatFakePathToUnixStyle(path_remove) << " from PATH.\n";
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

int shell_runExe(vector<string> args){
    if (args.size() < 2 || args.size() > 4) {
        printf("Usage: exec [path_or_name] [-b] [-c]\n");
        return BAD_COMMAND;
    }
    string input = args[1];
    string realPath;
    bool isPath = (input.find("\\") == 0 || input.find("/") == 0);
    // cout << "getNormalizedFilePath: " << getNormalizedFilePath(input) << "\n";
    // cout << "current_real_path is: " << current_real_path << "\n";
    // cout << "current_real_path + \\ + input : " << current_real_path + "\\" + input << "\n";
    if(!isPath){
        string combinedPath1 = current_real_path + "\\" + input;
        string combinedPath2 = current_real_path + "\\" + input + ".exe";
        char fullPath[MAX_PATH];
        DWORD file1 = GetFileAttributesA(combinedPath1.c_str());
        DWORD file2 = GetFileAttributesA(combinedPath2.c_str());
        if (file1 != INVALID_FILE_ATTRIBUTES && !(file1 & FILE_ATTRIBUTE_DIRECTORY)) {
            // realPath = current_real_path + "\\" + input;
            // cout << "realPath iss: +++++ " << realPath << "\n";
            // if(realPath.find(current_real_path) != 0){
            //     printf("Error: not found: ", input.c_str());
            //     return BAD_COMMAND;
            // }
            GetFullPathNameA(combinedPath1.c_str(), MAX_PATH, fullPath, NULL);
            realPath = fullPath;
            if (realPath.find(current_real_path) != 0) {
                printf("Error: not found: %s\n", input.c_str());
                return BAD_COMMAND;
            }
        }
        else if (file2 != INVALID_FILE_ATTRIBUTES && !(file2 & FILE_ATTRIBUTE_DIRECTORY)) {
            GetFullPathNameA(combinedPath2.c_str(), MAX_PATH, fullPath, NULL);
            realPath = fullPath;
            if (realPath.find(current_real_path) != 0) {
                printf("Error: not found: %s\n", input.c_str());
                return BAD_COMMAND;
            }
        }
        else{
            bool found = false;
            for (const string& dir : envPaths) {
                string rPath = getNormalizedDirectory(dir);
                string testPath = rPath + "\\" + input;
                string testPathWithExe = testPath + ".exe";
                //exe
                DWORD fileAttr = GetFileAttributesA(testPathWithExe.c_str());
                if (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                    realPath = testPathWithExe;
                    found = true;
                    break;
                }
                //bat
                string testPathWithBat = testPath + ".bat";
                fileAttr = GetFileAttributesA(testPathWithBat.c_str());
                if (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                    realPath = testPathWithBat;
                    found = true;
                    break;
                }
                fileAttr = GetFileAttributesA(testPath.c_str());
                if (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                    realPath = testPath;
                    found = true;
                    break;
                }
            }
            if (!found) {
                printf("Executable '%s' not found in current directory or envPaths.\n", formatFakePathToUnixStyle(input));
                return BAD_COMMAND;
            }
        }
    }
    else{
        realPath = getNormalizedFilePath(input);
        if (realPath.find(origin_real_path + "\\root") != 0) {
                // printf("Error: not found: %s\n",  formatFakePathToUnixStyle(input));
                return BAD_COMMAND;
            }
    }
    // printf("Attempting to execute: %s\n", realPath.c_str());
    DWORD fileAttr = GetFileAttributesA(realPath.c_str());
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        printf("File does not exist: %s\n", formatFakePathToUnixStyle(input));
        return BAD_COMMAND;
    }
    if (fileAttr & FILE_ATTRIBUTE_DIRECTORY) {
        printf("Path is a directory, not a file: %s\n", formatFakePathToUnixStyle(input));
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
        string cmdLine = realPath;
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
        // printf("Child process created with PID: %lu\n", pi.dwProcessId);

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
       vector<string> script_lines;
        if (!read_script_file(realPath, script_lines)) {
            return UNABLE_TO_OPEN_SCRIPT_FILE;
        }
        Node* root = build(script_lines);
        if (root) {
            root->execute();
            delete root;
        }
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

    // const char* path_env = getenv("PATH");
    // cout << path_env << endl;
    if (envPaths.empty()) {
        cout << "Failed to retrieve PATH environment variable.\n";
        return BAD_COMMAND;
    }

    // vector<string> directories = split_path(path_env);
    vector<string> path_tiny_shell;
    bool found = false;
    // kiem tra trong thu muc hien tai
    string test_path = current_real_path + "\\" + input;
    vector<string> ex = {"", ".exe", ".bat"};
    for (const string& v : ex) {
        string fullPath = test_path + v;
        DWORD fileAttr = GetFileAttributesA(fullPath.c_str());
        if (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
            string relative_path = removePrefix(fullPath, origin_real_path);
            string unix_path = formatFakePathToUnixStyle(relative_path);
            cout << unix_path << "\n";
            found = true;
        }
    }
    // Kiem tra trong env
    for (const string& dir : envPaths) {
        // cout << ++i << endl;
        // cout << "dir is : " << dir << endl;
        // if (isPrefix(dir, fixed_real_path) || isPrefix(dir, current_fake_path) || isPrefix(origin_real_path + dir, fixed_real_path)) {
        //     string relative_path = removePrefix(dir, origin_real_path);
        //     // cout << "relative_path is " << relative_path << endl;
        //     string unix_relative_path = formatFakePathToUnixStyle(relative_path);
        //     path_tiny_shell.push_back(unix_relative_path);
        // }
        string realDir = getNormalizedDirectory(dir);
        string test_path = realDir + "\\" + input;
        vector<string> ex = {"", ".exe", ".bat"};
        for (const string& v : ex) {
            string fullPath = test_path + v;
            DWORD fileAttr = GetFileAttributesA(fullPath.c_str());
            if (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                string relative_path = removePrefix(fullPath, origin_real_path);
                string unix_path = formatFakePathToUnixStyle(relative_path);
                cout << unix_path << "\n";
                found = true;
            }
        }
    }

    // for(const string& dir : path_tiny_shell){
    //     string test_path = origin_real_path + dir + "\\" + input;
    //     vector<string> ex = {"", ".exe", ".bat"};
    //     for (const string& v : ex){
    //         string fullPath = test_path + v;
    //         DWORD fileAttr = GetFileAttributesA(fullPath.c_str());
    //         if(fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)){
    //             cout << dir + "\\" + input + v << "\n";
    //             found = true;
    //         }
    //     }
    // }
    if(found == false){
        cout << "INFO: Could not find files for the given pattern(s).\n";
    }
    return 0;
}