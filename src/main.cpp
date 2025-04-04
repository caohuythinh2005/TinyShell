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
string ROOT_DIRECTORY = "/root";  // Định nghĩa thư mục gốc
string origin_real_path;
string current_real_path;
string current_fake_path;

#include "system_commands.h"
#include "constant.h"
#include "directory_manager.h"


vector<string> builtin_str = {
    // "help",
    // "list",
    "cls",
    "exit",
	"pwd"
	// "dir",
	// "cd"
};

int (*builtin_func[]) (vector<string>) = {
    // &shell_help,
    // &shell_print_processes_info,
    &shell_cls,
    &shell_exit,
	&shell_pwd
	// &shell_dir,
	// &shell_cd
};


int main() {
	init_system_commands();
	init_directory();
	string line;  /*command line*/ 
	vector<string> args; /*command line arguments*/
	while (status) {
		std::cout << "\033[32mmy_shell:\033[36m" << current_fake_path << "\033[0m$ \033[0m";
		line = read_command_line();
		args = parse_command(line);
		for (string x : args) {
			cout << "|" << x << "|" << endl;
		}
		int i = 0;
		if (args.size() > 0) {
			shell_working(args);
		}
	}
	return 0;
}


