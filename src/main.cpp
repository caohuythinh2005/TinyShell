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

// foreground mode
HANDLE fore = NULL;


#include "system_commands.h"
#include "constant.h"
#include "directory_manager.h"
#include "utils.h"
#include "path_manager.h"

vector<string> builtin_str = {
    // "list",
    "cls",
    "exit",
	"pwd",
	"dir",
	"cd",
	"mkdir",
	"del",
	"help",
	"path",
	"addpath",
	"set",
	"runExe",
	"delpath",
	"where"
};

int (*builtin_func[]) (vector<string>) = {
    // &shell_print_processes_info,
    &shell_cls,
    &shell_exit,
	&shell_pwd,
	&shell_dir,
	&shell_cd,
	&shell_mkdir,
	&shell_del,
	&shell_help,
	&shell_path,
	&shell_addpath,
	&shell_set,
	&shell_runExe,
	&shell_delpath,
	&shell_where
};


int main() {
	init_system_commands();
	init_directory();
	string line;  /*command line*/ 
	vector<string> args; /*command line arguments*/
	while (status) {
		std::cout << "\033[32mmy_shell:\033[36m" << formatFakePathToUnixStyle(current_fake_path) << "\033[0m$ \033[0m";
		line = read_command_line();
		args = parse_command(line);
		// for (string x : args) {
		// 	cout << "|" << x << "|" << endl;
		// }
		// int i = 0;
		if (args.size() > 0) {
			shell_working(args);
		}
		// char buffer[MAX_PATH];
		// GetCurrentDirectory(MAX_PATH, buffer);
		// printf("\n%s\n", buffer);
		// cout << origin_real_path << endl;
		// cout << current_real_path << endl;
		// cout << current_fake_path << endl << endl; 
	}
	return 0;
}


