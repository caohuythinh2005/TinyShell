#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <sys/stat.h>


#include "globals.h"



// global var
int status; /*flag to determine when to exit program*/
STARTUPINFO si;
PROCESS_INFORMATION pi;
string ROOT_DIRECTORY = "\\root";  // Định nghĩa thư mục gốc
string origin_real_path; // đến trước root thôi (ví dụ C:/root thì thành C:)
string fixed_real_path; // có thêm /root so với cái trên
string current_real_path;  // là (C:/root)
string current_fake_path; // là /root

// Xử lý hàng đợi
HANDLE hJob;
JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli;


#include "process.h"
#include "system_commands.h"
#include "constant.h"
#include "directory_manager.h"
#include "utils.h"



vector<string> builtin_str = {
    "list",
    "cls",
    "exit",
	"pwd",
	"dir",
	"cd",
	"mkdir",
	"del",
	"help",
	"test",
	"exec",
	"time",
	"date",
	"runExe",
	"kill_id",
	"pause_id",
	"resume_id"
};

int (*builtin_func[]) (vector<string>) = {
    &shell_list,
    &shell_cls,
    &shell_exit,
	&shell_pwd,
	&shell_dir,
	&shell_cd,
	&shell_mkdir,
	&shell_del,
	&shell_help,
	&shell_test,
	&shell_runScript,
	&shell_time,
	&shell_date,
	&shell_runExe,
	&shell_killProcessById,
	&shell_suspendById,
	&shell_resumeById
};


int main() {
	init_process();
	init_system_commands();
	init_directory();
	string line;  /*command line*/ 
	vector<string> args; /*command line arguments*/
	// if (!CreateProcess(
	// 	NULL,
	// 	(LPSTR)"notepad.exe",
	// 	NULL,
	// 	NULL,
	// 	FALSE,
	// 	CREATE_SUSPENDED, // Đảm bảo gán trước khi đưa vào tiến trình
	// 	NULL,
	// 	NULL,
	// 	&si,
	// 	&pi
	// )) {
	// 	cerr << "Failed to create process\n";
    //     CloseHandle(hJob);
    //     return 1;
	// }
	// /*
	// 	AssignProcessToJobObject(hJob, pi.hProcess)
	// 	Gán tiến trình con (được biểu thị bằng pi.hProcess) vào một Job Object 
	// 	đã được tạo trước đó (biểu thị bởi hJob).
	// */

	// if (!AssignProcessToJobObject(hJob, pi.hProcess)) {
	// 	cerr << "Failed to assign process to job\n";
	// 	TerminateProcess(pi.hProcess, 1);
	// 	CloseHandle(pi.hProcess);
	// 	CloseHandle(pi.hThread);
	// 	CloseHandle(hJob);
	// 	return 1;
	// }

	// ResumeThread(pi.hThread);
    // std::cout << "Press Enter to exit shell (notepad will also close)...\n";
    // std::cin.get(); // chờ người dùng nhấn Enter
	// CloseHandle(pi.hProcess);
	// CloseHandle(pi.hThread);
	// CloseHandle(hJob);
	while (status) {
		std::cout << "\033[32mmy_shell:\033[36m" << formatFakePathToUnixStyle(current_fake_path) << "\033[0m$ \033[0m";
		line = read_command_line();
		args = parse_command(line);
		if (args.size() > 0) {
			shell_working(args);
		}
	}
	return 0;
}


