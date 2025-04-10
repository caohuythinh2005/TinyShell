// #include <Windows.h>
// #include <TlHelp32.h>
// #include <tchar.h>


// // Cấu trúc của STARTUPINFO

// // typedef struct _STARTUPINFO {
// //     DWORD  cb;                // Kích thước của cấu trúc (phải gán = sizeof(STARTUPINFO))
// //     LPTSTR lpReserved;        // Dự trữ, phải đặt NULL
// //     LPTSTR lpDesktop;         // Tên desktop (NULL = mặc định)
// //     LPTSTR lpTitle;           // Tiêu đề cửa sổ nếu là GUI
// //     DWORD  dwX;               // Vị trí X của cửa sổ mới (nếu có)
// //     DWORD  dwY;               // Vị trí Y của cửa sổ mới (nếu có)
// //     DWORD  dwXSize;           // Chiều rộng cửa sổ
// //     DWORD  dwYSize;           // Chiều cao cửa sổ
// //     DWORD  dwXCountChars;     // Số ký tự theo chiều ngang trong cửa sổ console
// //     DWORD  dwYCountChars;     // Số ký tự theo chiều dọc trong cửa sổ console
// //     DWORD  dwFillAttribute;   // Thuộc tính màu nền chữ của console
// //     DWORD  dwFlags;           // Các cờ điều khiển cách khởi động tiến trình
// //     WORD   wShowWindow;       // Trạng thái hiển thị cửa sổ (ẩn, tối đa hóa, v.v.)
// //     WORD   cbReserved2;       // Dự trữ, phải đặt 0
// //     LPBYTE lpReserved2;       // Dự trữ, phải đặt NULL
// //     HANDLE hStdInput;         // Handle của đầu vào tiêu chuẩn (stdin)
// //     HANDLE hStdOutput;        // Handle của đầu ra tiêu chuẩn (stdout)
// //     HANDLE hStdError;         // Handle của lỗi tiêu chuẩn (stderr)
// // } STARTUPINFO, *LPSTARTUPINFO;


// // Cấu trúc của PROCESS_INFORMATION

// // typedef struct _PROCESS_INFORMATION {
// //     HANDLE hProcess;    // Handle của tiến trình mới
// //     HANDLE hThread;     // Handle của luồng chính của tiến trình
// //     DWORD  dwProcessId; // ID của tiến trình mới
// //     DWORD  dwThreadId;  // ID của luồng chính của tiến trình
// // } PROCESS_INFORMATION, *LPPROCESS_INFORMATION;

// // Cấu trúc của PROCESSENTRY32

// // typedef struct tagPROCESSENTRY32 {
// //     DWORD   dwSize;            // Kích thước của cấu trúc (phải được thiết lập trước khi sử dụng)
// //     DWORD   cntUsage;          // Số lượng handles đến tiến trình này
// //     DWORD   th32ProcessID;     // ID của tiến trình
// //     ULONG_PTR th32DefaultHeapID; // ID heap mặc định của tiến trình (không còn được dùng)
// //     DWORD   th32ModuleID;      // ID module (chỉ dùng trên Win9x, không còn quan trọng)
// //     DWORD   cntThreads;        // Số lượng threads thuộc tiến trình
// //     DWORD   th32ParentProcessID; // ID của tiến trình cha (parent process)
// //     LONG    pcPriClassBase;    // Mức ưu tiên của tiến trình
// //     DWORD   dwFlags;           // Không sử dụng
// //     TCHAR   szExeFile[MAX_PATH]; // Tên file thực thi (exe) của tiến trình
// //   } PROCESSENTRY32;


// int main() {
// 	printf("--------PROCESS LISTING--------\n");
//     HANDLE hSnapShot = INVALID_HANDLE_VALUE;
//     PROCESSENTRY32 ProcessInfo = {0};
//     ProcessInfo.dwSize = sizeof(PROCESSENTRY32);
//     hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//     int count = 0;
//     if (INVALID_HANDLE_VALUE == hSnapShot)
//     {
//         printf("Error...\n");
//         return -1;
//     }
//     printf("Create tool success...\n");
   
//     while (Process32Next(hSnapShot, &ProcessInfo)){
//         if ( ProcessInfo.th32ParentProcessID == GetCurrentProcessId() || ProcessInfo.th32ProcessID == GetCurrentProcessId() )
//         {
//         _tprintf(TEXT("\n\n====================================================="));
//         _tprintf(TEXT("\n\tPROCESS NO:          %d"), ++count);
//         _tprintf(TEXT("\n\tNO. OF THREAD:       %d"), ProcessInfo.cntThreads);
//         _tprintf(TEXT("\n\tSIZE:                %d"), ProcessInfo.dwSize);
//         _tprintf(TEXT("\n\tBASE PRIORITY:       %d"), ProcessInfo.pcPriClassBase);
//         _tprintf(TEXT("\n\tPROCESS NAME:        %s"), ProcessInfo.szExeFile);
//         _tprintf(TEXT("\n\tPARENT PROCESS ID:   %d"), ProcessInfo.th32ParentProcessID);
//         _tprintf(TEXT("\n\tPROCESS ID:          %d"), ProcessInfo.th32ProcessID);
//         }
//     }
//     printf("\n\n");
//     CloseHandle(hSnapShot);
//     return 0;
// }