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

#include "directory_manager.h"
#include "constant.h"
#include "utils.h"
// #include "Shlwapi.h"





int init_directory() {
    // Lấy đường dẫn thư mục hiện tại
    char buffer[MAX_PATH];
    if (GetCurrentDirectory(MAX_PATH, buffer)) {
        current_real_path = buffer;
        origin_real_path = buffer;
    } else {
        cerr << "Lỗi khi lấy đường dẫn thư mục hiện tại." << endl;
        return -1;
    }

    // Thêm "/root" vào cuối current_real_path
    current_real_path += "\\root";
    current_fake_path = "\\root";  // Gán đường dẫn giả
    fixed_real_path = current_real_path;
    return 0;
}

bool deleteRecursive(const string& path) {
    WIN32_FIND_DATA fd;
    string searchPath = path + "\\*";
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &fd);

    if (hFind == INVALID_HANDLE_VALUE)
        return false;

    do {
        string item = fd.cFileName;
        if (item == "." || item == "..") continue;

        string fullPath = path + "\\" + item;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!deleteRecursive(fullPath)) {
                FindClose(hFind);
                return false;
            }
        } else {
            if (!DeleteFile(fullPath.c_str())) {
                FindClose(hFind);
                return false;
            }
        }
    } while (FindNextFile(hFind, &fd));

    FindClose(hFind);
    return RemoveDirectory(path.c_str());
}
/*
Xóa foder và các file bên trong ngay tại đường dẫn của nó
*/

int shell_del(vector<string> args) {
    if (args.size() != 2) {
        cout << "ERROR: Command 'del' requires exactly one argument" << endl;
        cout << "Usage: del [dir]" << endl;
        return BAD_COMMAND;
    }

    SetCurrentDirectory(current_real_path.c_str());
    string target = args[1];

    DWORD attr = GetFileAttributes(target.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) {
        cout << "ERROR: Directory not found." << endl;
        SetCurrentDirectory(origin_real_path.c_str());
        return BAD_COMMAND;
    }

    bool success = false;
    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        success = deleteRecursive(target);
        if (!success) {
            cout << "ERROR: Unable to delete folder or its contents." << endl;
        }
    } else {
        success = DeleteFile(target.c_str());
        if (!success) {
            cout << "ERROR: Unable to delete file." << endl;
        }
    }

    SetCurrentDirectory(origin_real_path.c_str());
    return success ? 0 : BAD_COMMAND;
}

int shell_dir(vector<string> args) {
	if (args.size() > 1) {
        printf("\nBad command ... \n");
        return BAD_COMMAND;
    }

    char current_real_path_cstr[MAX_PATH];
    strncpy(current_real_path_cstr, current_real_path.c_str(), MAX_PATH);

    WIN32_FIND_DATA findFileData;
    char searchPath[MAX_PATH];

    // Tạo đường dẫn tìm kiếm trong thư mục hiện tại
    snprintf(searchPath, MAX_PATH, "%s\\*", current_real_path_cstr);

    HANDLE hFind = FindFirstFile(searchPath, &findFileData);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Không thể đọc thư mục (%s).\n", current_real_path_cstr);
        return 1;  // Báo lỗi
    }
    
    do {
        // Bỏ qua "." và ".." để làm cho shell dễ dùng hơn :))
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0)
            continue;

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            printf("[DIR]  %s\n", findFileData.cFileName);
        } else {
            printf("[FILE] %s\n", findFileData.cFileName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    
    FindClose(hFind);
    return 0;
}

int shell_pwd(vector<string> args) {
	if (args.size() > 1) {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }
	cout << formatFakePathToUnixStyle(current_fake_path) << endl;
	return 0;
}

/*
Chỉ cho phép tạo folder trong directory hiện tại
*/
int shell_mkdir(vector<string> args) {
    if (args.size() != 2) {
        printf("Usage: mkdir <foldername>\n");
        return BAD_COMMAND;
    }

    string full_path = current_real_path + "\\" + args[1];
    
    if (!CreateDirectory(full_path.c_str(), NULL)) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            printf("Directory already exists.\n");
        } else {
            printf("Failed to create directory.\n");
        }
    }

    return 0;
}

// int pathFileExists(string path) {
//     char new_path[1024];

//     // **1. Xử lý đường dẫn tuyệt đối**
//     if (path[0] == '/') {

// 		char tmp[2048];
// 		strcpy(tmp, origin_real_path.c_str());
// 		strcat(tmp, path.c_str());
//         // int retval = PathFileExists(tmp);
//         // if (retval == 1) {
//         //     return 0;
//         // }
// 		// check path
//     }
//     return FILE_NOT_EXITS;
// }


/*
Để tránh rắc rối, ta luôn mặc định current_directory (đang làm việc) là toàn bộ TinyShell
extern string origin_real_path; -> Lưu trữ cái trên
extern string current_real_path; -> Lưu trữ đường dẫn đang giả lập
extern string current_fake_path; -> Lưu trữ đường dẫn đang giả lập, với cây thư mục của bản thân
Khi thực hiện cd chỉ thay đổi hai cái dưới
khi tạo ta sẽ chỉ định trực tiếp xong lại thay về origin_real_path
Làm thế này đỡ phải check đường dẫn tương đối vì hiển nhiên phải tiếp theo từ origin
Chỉ cho phép đường dẫn tuyệt đối từ /root/...
*/

int shell_cd(vector<string> args) {
    if (args.size() == 1) return 0;
    if (args.size() == 2) {
        // Lệnh "cd" có tham số
        string path_str = args[1];
        if (path_str[0] == '/' || path_str[0] == '\\') {
            if (path_str.size() >= 5 && (path_str.substr(0, 5) == "/root" || path_str.substr(0, 5) == "\\root")) {
                char full_path[MAX_PATH];
                strcpy(full_path, origin_real_path.c_str());
                strcat(full_path, path_str.c_str());
                if (SetCurrentDirectory(full_path) == FALSE) {
                    cout << "No such directory" << endl;
                    SetCurrentDirectory(origin_real_path.c_str());
                    return DIRECTORY_NOT_EXISTS;
                }
                string fullPath = getNormalizedCurrentDirectory();
                SetCurrentDirectory(origin_real_path.c_str());
                current_real_path = fullPath;
                current_fake_path = removePrefix(current_real_path, origin_real_path);
                return 0;
            } else {
                printf("Bad command ... \n");
                return BAD_COMMAND;
            }
        } else if (path_str == "..") {
            if (current_fake_path == "\\root") {
                return 0;
            }
            // Ngược lại thì ok rồi :))
            char current_real_path_c_str[MAX_PATH];
            char current_fake_path_c_str[MAX_PATH];
            strcpy(current_real_path_c_str, current_real_path.c_str());
            strcpy(current_fake_path_c_str, current_fake_path.c_str());
            char* last_real_slash = strrchr(current_real_path_c_str, '\\');
            char* last_fake_slash = strrchr(current_fake_path_c_str, '\\');
            *last_real_slash = '\0';
            *last_fake_slash = '\0';
            current_real_path = current_real_path_c_str;
            current_fake_path = current_fake_path_c_str;
            return 0;
        }  if (path_str == ".") {
            return 0;
        } else {
            // Di chuyển theo đường dẫn tương đối
            char full_path[MAX_PATH];
            strcpy(full_path, current_real_path.c_str());
            strcat(full_path, "\\");
            strcat(full_path, path_str.c_str());
            if(SetCurrentDirectory(full_path)==FALSE){
                cout << "No such directory" << endl;
                SetCurrentDirectory(origin_real_path.c_str());
                return DIRECTORY_NOT_EXISTS;
            } else {
                string fullPath = getNormalizedCurrentDirectory();
                if (isPrefix(fullPath, fixed_real_path)) {
                    SetCurrentDirectory(origin_real_path.c_str());
                    current_real_path = fullPath;
                    current_fake_path = removePrefix(current_real_path, origin_real_path);
                }
                else {
                    printf("Bad command ... \n");
                    return BAD_COMMAND;
                }
            } 
            return 0;
        }

    } else {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }
    return 0;
}
