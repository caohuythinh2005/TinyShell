#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <sys/stat.h>


#include "utilities/globals.h"
#include "execution/process.h"
#include "execution/system_commands.h"
#include "utilities/constant.h"
#include "filesystem/directory_manager.h"
#include "utilities/utils.h"
#include "filesystem/path_manager.h"
#include "utilities/cal.h"
#include "parsing/condition_evaluator.h"
#include "variables/variable_manager.h"
#include "parsing/script_io.h"
#include "parsing/ast/node.h"
#include "parsing/ast/if_node.h"
#include "parsing/ast/block_node.h"
#include "parsing/ast/command_node.h"
#include "parsing/ast/gb.h"
#include "parsing/ast/builder.h"
#include "editor/editor.h"



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


int main1(vector<string> args);



vector<string> builtin_str = {
    "cls",
    "exit",
	"pwd",
	"dir",
	"cal",
	"cd",
	"mkdir",
	"del",
	"help",
	"test",
	"time",
	"date",
	"exec",
	"kill_id",
	"pause_id",
	"resume_id",
	"list",
	"touch",
	"cat",
	"write",
	"rename",
	"move",
	"copy",
	"path",
	"addpath",
	"addpathx",
	"delpath",
	"delpathx",
	"where",
	"echo",
	"set",
	"setx",
	"eval",
	"editor"
};

int (*builtin_func[]) (vector<string>) = {
    &shell_cls,
    &shell_exit,
	&shell_pwd,
	&shell_dir,
	&shell_cal,
	&shell_cd,
	&shell_mkdir,
	&shell_del,
	&shell_help,
	&shell_test,
	&shell_time,
	&shell_date,
	&shell_runExe,
	&shell_killProcessById,
	&shell_suspendById,
	&shell_resumeById,
	&shell_list,
	&shell_touch,
	&shell_cat,
	&shell_write,
	&shell_rename,
	&shell_move,
	&shell_copy,
	&shell_path,
	&shell_addpath,
	&shell_addpathx,
	&shell_delpath,
	&shell_delpathx,
	&shell_where,
	&shell_echo,
	&shell_set,
	&shell_setx,
	&shell_eval,
	&shell_editor
};


int find_builtin(const string& cmd) {
    for (int i = 0; i < (int)builtin_str.size(); ++i) {
        if (builtin_str[i] == cmd) { // phân biệt hoa thường
            return i;
        }
    }
    return -1;
}

int main() {
	init_process();
	init_system_commands();
	init_directory();
	init_variable_manager("config\\.myshell_env");
	initPath();
	string line;  /*command line*/ 
	vector<string> args; /*command line arguments*/
	while (status) {
		std::cout << "\033[32mmy_shell:\033[36m" << formatFakePathToUnixStyle(current_fake_path) << "\033[0m$ \033[0m";
		line = read_command_line();
		args = parse_command(line);
		args = formatToRealPath(args);
		if (args.size() > 0) {
			shell_working(args);
		}
	}
	return 0;
}


