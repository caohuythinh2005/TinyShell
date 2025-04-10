#include "globals.h"
#include "process.h"
#include "constant.h"
#include "system_commands.h"
#include "directory_manager.h"
#include "utils.h"

/*
Hai trạng thái mà mình quan tâm

Running: Khi tiến trình đang thực thi bình thường.

Suspended: Khi tiến trình bị tạm dừng (suspend), không thực thi cho đến khi được tiếp tục.

*/


int init_process() {
    hJob = CreateJobObject(
        NULL, // Trỏ tới cấu trúc an ninh -> để NULL thì ai truy cập cũng được
        NULL  // Tên của Job
    );
    // Khi Job Object bị đóng thì toàn bộ tiến trình trong job bị terminate
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    // gán các thông số trong jeli vào hJob
    SetInformationJobObject(
        hJob,
        JobObjectExtendedLimitInformation,
        &jeli,
        sizeof(jeli)
    );
    si = {sizeof(STARTUPINFO)};
    pi = {0};

    // Tạo một Job Object ẩn danh.
    // Cài đặt giới hạn: kill tất cả tiến trình trong job nếu shell (job owner) bị tắt.
    // Apply cài đặt này vào Job Object.

    return 0;
}

// Tạm thời hai tham số đã
// đầu tiên là exec, thứ hai là đường dẫn
// Về sau hỗ trợ các mode sau

int shell_runScript(vector<string> args)
{
    if (args.size() != 2)
    {
        cout << "Bad command...\n";
        return BAD_COMMAND;
    }
    // Hỗ trợ env sau
    string scriptPath = convertFakeToRealPath(args[1]);
    // Kiểm tra xem file script có tồn tại không

    int check = fileExists(args[1]);
    if (check == ERROR_PATH) {
        printf("ERROR_PATH\n");
        return ERROR_PATH;
    } else if (check == FILE_NOT_EXIST) {
        printf("FILE_NOT_EXIST\n");
        return FILE_NOT_EXIST;
    };
    ifstream scriptFile(scriptPath);
    if (!scriptFile)
    {
        cout << "UNABLE TO OPEN SCRIPT FILE: " << scriptPath << endl;
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

// void sigintHandler(int sig_num) {
//     // Nếu có tiến trình foreground đang chạy, terminate nó
//     if (fore != NULL) {
//         std::cout << "\nInterrupt received. Terminating foreground process...\n";
//         TerminateProcess(fore, 1); // Dừng tiến trình
//         fore = NULL;  // Xóa handle
//     }
//     std::cin.clear(); // Đảm bảo không bị lỗi khi đọc từ stdin
// }

// Chạy trực tiếp tại thư mục hoặc là đường dẫn

// int shell_runExe(vector<string> args) {
//     if (args.size() < 2 || args.size() > 4) {
//         printf("Usage: runExe [path] [-b] [-c]\n");
//         return BAD_COMMAND;
//     }

//     string realPath = "";

//     string inputPath = args[1];
    
//     if ((inputPath[0] == '\\' || inputPath[0] == '/') && 
//              (inputPath.substr(0, 5) == "\\root" || inputPath.substr(0, 5) == "/root")) {
//         if (fileExists(inputPath) != EXIST_FILE_OR_DIRECTORY) {
//             std::cerr << "The specified file path does not exist. Please check the path and try again.\n";
//             return -1;
//         }
//         realPath = convertFakeToRealPath(inputPath);
//     }
//     else {
//         inputPath = current_fake_path + "\\"  + args[1];
//         if (fileExists(inputPath) != EXIST_FILE_OR_DIRECTORY) {
//             std::cerr << "The specified file path does not exist. Please check the path and try again.\n";
//             return -1;
//         }
//         realPath = convertFakeToRealPath(inputPath);
//     }
//     string cmdLine = realPath;
//     STARTUPINFOA si = { sizeof(si) };
//     PROCESS_INFORMATION pi;
//     DWORD creationFlags = CREATE_UNICODE_ENVIRONMENT;
//     bool isBackground = false;
//     bool createConsole = false;

//     // Kiểm tra flag
//     for (int i = 2; i < args.size(); ++i) {
//         if (args[i] == "-b") isBackground = true;
//         else if (args[i] == "-c") createConsole = true;
//         else {
//             printf("Unknown flag: %s\n", args[i].c_str());
//             return BAD_COMMAND;
//         }
//     }

//     HANDLE hNull = NULL;
//     if (isBackground && !createConsole) {
//         // Chặn stdout/stderr khi chạy nền không có console mới
//         // "NUL" là một thiết bị out
//         // Mục đích chính là sẽ nuốt dữ liệu in ra
//         hNull = CreateFileA("NUL", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//         if (hNull != INVALID_HANDLE_VALUE) {
//             si.dwFlags |= STARTF_USESTDHANDLES;
//             si.hStdOutput = hNull;
//             si.hStdError  = hNull;
//         }
//     }

//     if (createConsole) {
//         creationFlags |= CREATE_NEW_CONSOLE;
//     }

//     BOOL success = CreateProcessA(
//         NULL,
//         cmdLine.data(),
//         NULL,
//         NULL,
//         TRUE,  // Kế thừa handle nếu cần
//         creationFlags,
//         NULL,
//         NULL,
//         &si,
//         &pi
//     );

//     if (hNull) CloseHandle(hNull);  // đóng sau khi CreateProcess dùng xong

//     if (!success) {
//         cerr << "Failed to create process. Error code: " << GetLastError() << "\n";
//         return 1;
//     }

//     std::signal(SIGINT, sigintHandler);
//     AssignProcessToJobObject(hJob, pi.hProcess);

//     if (!isBackground) {
//         WaitForSingleObject(pi.hProcess, INFINITE);
//         // printf("Child Complete\n");
//     }

//     CloseHandle(pi.hProcess);
//     CloseHandle(pi.hThread);

//     return 0;
// }

// Cài đệ quy phục vụ sandbox
 
void addProcesses(DWORD parentPID, vector<ProcessInfor>& result) {
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapShot, &pe)) {
        do {
            // Kiểm tra nếu tiến trình là con của tiến trình gốc
            if (pe.th32ParentProcessID == parentPID) {
                ProcessInfor info;
                info.pid = pe.th32ProcessID;
                info.processName = pe.szExeFile;
                info.status = "running";  // Cần phải tìm cách lấy status thực tế
                // Thêm vào danh sách kết quả
                result.push_back(info);

                // Đệ quy để lấy tiếp tiến trình con của tiến trình này
                addProcesses(pe.th32ProcessID, result);
            }
        } while (Process32Next(hSnapShot, &pe));
    }
    CloseHandle(hSnapShot);
}

vector<ProcessInfor> getShellProcesses() {
    vector<ProcessInfor> result;  // Đảm bảo danh sách result được làm mới

    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == INVALID_HANDLE_VALUE) {
        cout << "Failed to create snapshot.\n";
        return result;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    DWORD shellPID = GetCurrentProcessId();

    // Bắt đầu từ tiến trình gốc (shellPID)
    addProcesses(shellPID, result);
    CloseHandle(hSnapShot);
    return result;
}

/*
Lưu ý : bool isProcessSuspended(DWORD pid)
- Trả về true nếu tất cả các thread của tiến trình đã bị suspend.
- Trả về false nếu:
  + Bất kỳ thread nào vẫn đang chạy (suspendCount == 0)
  + Không thể mở handle tới thread (OpenThread fail)
  + Không thể gọi SuspendThread
*/

bool isProcessSuspended(DWORD pid) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;

    THREADENTRY32 te;
    te.dwSize = sizeof(te);

    bool hasThreads = false;
    bool allSuspended = true;

    if (Thread32First(hSnapshot, &te)) {
        do {
            if (te.th32OwnerProcessID == pid) {
                hasThreads = true;
                HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if (hThread) {
                    // thử resume rồi suspend lại để ước lượng trạng thái
                    DWORD suspendCount = SuspendThread(hThread);
                    ResumeThread(hThread); // Trả lại trạng thái ban đầu

                    if (suspendCount == (DWORD)-1 || suspendCount == 0) {
                        allSuspended = false;
                    }

                    CloseHandle(hThread);
                } else {
                    allSuspended = false;
                }
            }
        } while (Thread32Next(hSnapshot, &te));
    }

    CloseHandle(hSnapshot);
    return hasThreads && allSuspended;
}

vector<ProcessInfor> getShellProcessesWithStatus() {
    vector<ProcessInfor> processes = getShellProcesses();
    for (int i = 0; i < processes.size(); i++) {
        // for (int j = 0; j < lstProcessSuspended.size(); j++) {
        //     if (processes[i].pid == lstProcessSuspended[j]) {
        //         processes[i].status = "suspended";
        //     }
        // }
        if (isProcessSuspended(processes[i].pid)) {
            processes[i].status = "suspended";
        } else {
            processes[i].status = "running";
        }
    }
    return processes;
}

void printProcesses(const vector<ProcessInfor>& processes) {
    cout << "-----------------------------------------\n";
    cout << "PID\t\tName\t\tStatus\n";
    cout << "-----------------------------------------\n";

    for (const auto& p : processes) {
        // Lọc bỏ những tiến trình không cần thiết (ví dụ như "conhost.exe")
        if (strcmp(p.processName.c_str(), "conhost.exe") == 0) {
            continue;  // Bỏ qua các tiến trình conhost.exe
        }

        // In ra thông tin tiến trình
        cout << p.pid << "\t\t" << p.processName << "\t\t" << p.status << endl;
    }

    cout << "-----------------------------------------\n";
}

int shell_killProcessById(vector<string> args) {
    if (args.size() != 2) {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }

    DWORD pidToKill = stoul(args[1]);
    
    vector<ProcessInfor> processes = getShellProcesses();
    // Tìm kiếm ProcessInfo có pid trùng
    // Làm thế này để đảm bảo sandbox
    bool found = false;
    for (auto& process : processes) {
        if (process.pid == pidToKill) {
            found = true;
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, (DWORD) process.pid);
            // Tiến hành kill process
            if (TerminateProcess(hProcess, 1)) {
                cout << "Process " << pidToKill << " terminated.\n";
                CloseHandle(hProcess);
                process.status = "terminated";
            } else {
                cerr << "Failed to terminate process.\n";
            }
            break; // Dừng vòng lặp nếu tìm thấy PID
        }
    }

    if (!found) {
        cout << "Bad PID" << endl;
        return -1;
    }
    return 0;
}

int shell_suspendById(vector<string> args) {
    if (args.size() != 2) {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }
    vector<ProcessInfor> processes = getShellProcessesWithStatus();
    DWORD pidToSuspend = stoul(args[1]);
    // Tìm tiến trình cần suspend bằng vòng lặp thông thường
    // Neeys đã suspended thì bỏ qua
    // Thực tế windows có thể suspended chồng nhau -> tăng lên thôi
    bool found = false;
    ProcessInfor targetProcess;
    for (const auto& process : processes) {
        if (process.pid == pidToSuspend) {
            if (process.status == "suspended") {
                return 0;
            }
            targetProcess = process;
            found = true;
            break;
        }
    }
    if (found) {
        /*
        Lấy ThreadId của tất cả các thread thuộc tiến trình cần suspend.
        Mở từng thread với OpenThread(THREAD_SUSPEND_RESUME, ...).
        Gọi SuspendThread(hThread) đúng cách trên thread handle, không phải process handle.
        */
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        THREADENTRY32 te;
        te.dwSize = sizeof(THREADENTRY32);
        do
        {
            if (te.th32OwnerProcessID == pidToSuspend)
            {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if (hThread != NULL)
                {
                    SuspendThread(hThread);
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnapshot, &te));
        cout << "Process " << pidToSuspend << " suspended" << endl;
        CloseHandle(hSnapshot);
    } else {
        cout << "Bad PID" << endl;
        return -1;
    }
    return 0;
}

int shell_resumeById(vector<string> args) {
    if (args.size() != 2) {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }

    DWORD pidToSuspend = stoul(args[1]);
    // Tìm tiến trình cần suspend bằng vòng lặp thông thường
    bool found = false;
    vector<ProcessInfor> processes = getShellProcessesWithStatus();
    for (const auto& process : processes) {
        if (process.pid == pidToSuspend) {
            if (process.status == "suspended") {
                found = true;
                break;
            } else return 0;
        }
    }
    if (found) {
        /*
        Lấy ThreadId của tất cả các thread thuộc tiến trình cần suspend.
        Mở từng thread với OpenThread(THREAD_SUSPEND_RESUME, ...).
        Gọi ResumeThread(hThread) đúng cách trên thread handle, không phải process handle.
        */
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        THREADENTRY32 te;
        te.dwSize = sizeof(THREADENTRY32);
        do
        {
            if (te.th32OwnerProcessID == pidToSuspend)
            {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if (hThread != NULL)
                {
                    ResumeThread(hThread);
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnapshot, &te));
        cout << "Process " << pidToSuspend << " resumed" << endl;
        CloseHandle(hSnapshot);
    } else {
        cout << "Bad PID" << endl;
        return -1;
    }
    return 0;
}


int shell_list(vector<string> args) {
    printProcesses(getShellProcessesWithStatus());
    return 0;
}