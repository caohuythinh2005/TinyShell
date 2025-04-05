#include "globals.h"
#include "process.h"
#include "constant.h"
#include "system_commands.h"
#include "directory_manager.h"
#include "utils.h"


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

                // Mở tiến trình để có thể thao tác với nó
                info.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);


                // Thêm vào danh sách kết quả
                result.push_back(info);

                // Đệ quy để lấy tiếp tiến trình con của tiến trình này
                addProcesses(pe.th32ProcessID, result);
            }
        } while (Process32Next(hSnapShot, &pe));
    }
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
    // if (args.size() != 2) {
    //     cerr << "Usage: kill <ProcessID>\n";
    //     return -1;
    // }

    // DWORD pidToKill = stoul(args[1]);
    
    // // Tìm kiếm ProcessInfo có pid trùng
    // bool found = false;
    // for (auto& process : processList) {
    //     if (process.pid == pidToKill) {
    //         found = true;

    //         // Tiến hành kill process
    //         if (TerminateProcess(process.hProcess, 1)) {
    //             cout << "Process " << pidToKill << " terminated.\n";
    //             CloseHandle(process.hProcess);
    //             process.status = "terminated";
    //             // Hoặc remove khỏi danh sách nếu cần:
    //             // processList.erase(std::remove(processList.begin(), processList.end(), process), processList.end());
    //         } else {
    //             cerr << "Failed to terminate process.\n";
    //         }
    //         break; // Dừng vòng lặp nếu tìm thấy PID
    //     }
    // }

    // if (!found) {
    //     std::cerr << "PID " << pidToKill << " is not managed by TinyShell.\n";
    //     return -1;
    // }

    // return 0;
    return 0;
}

int shell_suspendById(vector<string> args) {
    return 0;
}

int shell_resumeById(vector<string> args) {
    return 0;
}

int shell_list(vector<string> args) {
    printProcesses(getShellProcesses());
}