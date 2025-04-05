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

void sigintHandler(int sig_num) {
    // Nếu có tiến trình foreground đang chạy, terminate nó
    if (fore != NULL) {
        std::cout << "\nInterrupt received. Terminating foreground process...\n";
        TerminateProcess(fore, 1); // Dừng tiến trình
        fore = NULL;  // Xóa handle
    }
    std::cin.clear(); // Đảm bảo không bị lỗi khi đọc từ stdin
}

int shell_runExe(vector<string> args) {
    // -b : background mode
    si.cb = sizeof(si);
    if (args.size() == 1) {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }
    if (args.size() == 4) {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }
    if (args.size() == 3 && args[2] != "-b") {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }

    if (!CreateProcess(
        NULL,
        (LPSTR)(convertFakeToRealPath(args[1])).data(),
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE | CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        printf("Cannot create process\n");
        return 0;
    } else {
        std::signal(SIGINT, sigintHandler);
        AssignProcessToJobObject(hJob, pi.hProcess);
        ResumeThread(pi.hThread);
        if (args.size() == 2) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            printf("Child Complete\n");
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else return 0;
    }
    return 0;
}

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

vector<ProcessInfor> getShellProcessesWithStatus() {
    vector<ProcessInfor> processes = getShellProcesses();
    for (int i = 0; i < processes.size(); i++) {
        for (int j = 0; j < lstProcessSuspended.size(); j++) {
            if (processes[i].pid == lstProcessSuspended[j]) {
                processes[i].status = "suspended";
            }
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

int suspend(DWORD processId) {
    return 0;
}

int resume(DWORD processId) {
    return 0;
}

// int shell_killProcessByName(vector<string> args) {
//     return 0;
// }

// int shell_suspendByName(vector<string> args) {
//     return 0;
// }

// int shell_resumeByName(vector<string> args) {
//     return 0;
// }

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
    checkAndRemoveClosedSuspendedProcesses();
    return 0;
}

int shell_suspendById(vector<string> args) {
    if (args.size() != 2) {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }

    DWORD pidToSuspend = stoul(args[1]);
    for (DWORD x : lstProcessSuspended) {
        if (x == pidToSuspend) return 0;
    }
    vector<ProcessInfor> processes = getShellProcesses();
    // Tìm tiến trình cần suspend bằng vòng lặp thông thường
    bool found = false;
    ProcessInfor targetProcess;
    for (const auto& process : processes) {
        if (process.pid == pidToSuspend) {
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
        lstProcessSuspended.push_back(pidToSuspend);
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
    for (DWORD process : lstProcessSuspended) {
        if (process == pidToSuspend) {
            found = true;
            break;
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
        for (auto it = lstProcessSuspended.begin(); it != lstProcessSuspended.end(); ) {
            DWORD pid = *it;
            if (pid == pidToSuspend) {
                lstProcessSuspended.erase(it);
                break;
            }
            ++it;
        }
        CloseHandle(hSnapshot);
    } else {
        cout << "Bad PID" << endl;
        return -1;
    }
    return 0;
}

void checkAndRemoveClosedSuspendedProcesses() {
    vector<ProcessInfor> processes = getShellProcesses();
    auto it = lstProcessSuspended.begin();
    while (it != lstProcessSuspended.end()) {
        DWORD pid = *it;
        bool check = false;
        for (ProcessInfor process: processes) {
            if (process.pid == pid) {
                check = true;
                break;
            }
        }
        if (check == false) {
            lstProcessSuspended.erase(it);
            ++it;
        }
    }
}



int shell_list(vector<string> args) {
    printProcesses(getShellProcessesWithStatus());
}