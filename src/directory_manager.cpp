#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <sys/stat.h>
#include "directory_manager.h"
#include "constant.h"




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
    current_real_path += "/root";
    current_fake_path = "/root";  // Gán đường dẫn giả
    return 0;
}

// int shell_dir(vector<string>) {
// 	if (args[1] != NULL) {
//         printf("\nBad command ... \n");
//         return BAD_COMMAND;
//     }
//     WIN32_FIND_DATA findFileData;
//     char searchPath[MAX_PATH];

//     // Tạo đường dẫn tìm kiếm trong thư mục hiện tại
//     snprintf(searchPath, MAX_PATH, "%s\\*", current_real_path);

//     HANDLE hFind = FindFirstFile(searchPath, &findFileData);
    
//     if (hFind == INVALID_HANDLE_VALUE) {
//         printf("Không thể đọc thư mục (%s).\n", current_real_path);
//         return 1;  // Báo lỗi
//     }
    
//     do {
//         // Bỏ qua "." và ".." để làm cho shell dễ dùng hơn :))
//         if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0)
//             continue;

//         if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
//             printf("[DIR]  %s\n", findFileData.cFileName);
//         } else {
//             printf("[FILE] %s\n", findFileData.cFileName);
//         }
//     } while (FindNextFile(hFind, &findFileData) != 0);
    
//     FindClose(hFind);
//     return 0;
// }

int shell_pwd(vector<string> args) {
	if (args.size() > 1) {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }
	cout << current_fake_path << endl;
	return 0;
}

// int pathFileExists(char* path) {
//     // char new_path[1024];

//     // // **1. Xử lý đường dẫn tuyệt đối**
//     // if (path[0] == '/') {

// 	// 	char tmp[2048];
// 	// 	strcpy(tmp, origin_real_path);
// 	// 	strcat(tmp, path);
//     //     int retval = PathFileExists(tmp);
//     //     if (retval == 1) {
//     //         return 0;
//     //     }
// 	// 	// check path
//     // }
//     // return FILE_NOT_EXITS;
// }

// int shell_cd(char **args) {
// 	// Hàm này khó vc
//     if (args[1] == NULL) {
//         printf("\nBad command ... \n");
//         return BAD_COMMAND;
//     }

//     char *path = args[1];
//     char new_path[1024];

//     // **1. Xử lý đường dẫn tuyệt đối**
//     if (path[0] == '/') {

// 		char tmp[2048];
// 		strcpy(tmp, origin_real_path);
// 		strcat(tmp, args[1]);

// 		// check path

// 		//
// 		strcpy(current_fake_path, args[1]);
// 		strcpy(current_real_path, tmp);
//     }
//     // // **2. Xử lý thư mục cha `..`**
//     // else if (strcmp(path, "..") == 0) {
//     //     if (getcwd(new_path, sizeof(new_path)) == NULL) {
//     //         perror("shell_cd: lỗi getcwd");
//     //         return -1;
//     //     }
//     //     char *last_slash = strrchr(new_path, '/');
//     //     if (last_slash != NULL) {
//     //         *last_slash = '\0';  // Cắt bỏ phần cuối để về thư mục cha
//     //     }
//     // }
//     // // **3. Xử lý thư mục hiện tại `.`**
//     // else if (strcmp(path, ".") == 0) {
//     //     if (getcwd(new_path, sizeof(new_path)) == NULL) {
//     //         perror("shell_cd: lỗi getcwd");
//     //         return -1;
//     //     }
//     // }
//     // // **4. Xử lý đường dẫn tương đối**
//     // else {
//     //     if (getcwd(new_path, sizeof(new_path)) == NULL) {
//     //         perror("shell_cd: lỗi getcwd");
//     //         return -1;
//     //     }
//     //     strncat(new_path, "/", sizeof(new_path) - strlen(new_path) - 1);
//     //     strncat(new_path, path, sizeof(new_path) - strlen(new_path) - 1);
//     // }

//     // // **Kiểm tra quyền truy cập (giới hạn trong ROOT_DIRECTORY)**
//     // if (!isValidPath(new_path)) {
//     //     fprintf(stderr, "shell_cd: Đường dẫn không hợp lệ, bạn chỉ có thể thao tác trong %s.\n", ROOT_DIRECTORY);
//     //     return -1;
//     // }

//     // // **Chuyển thư mục**
//     // if (chdir(new_path) != 0) {
//     //     perror("shell_cd: không thể chuyển thư mục");
//     //     return -1;
//     // }

//     // printf("Đã chuyển đến thư mục: %s\n", new_path);
//     // return 0;
// }

