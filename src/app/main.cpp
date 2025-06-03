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
#include "execution/color_command.h"


void loadConfig(const std::string& filename);
void saveConfig(const std::string& filename);
// global var
int status; /*flag to determine when to exit program*/
STARTUPINFO si;
PROCESS_INFORMATION pi;
string ROOT_DIRECTORY = "\\root";  // Định nghĩa thư mục gốc
string origin_real_path; // đến trước root thôi (ví dụ C:/root thì thành C:)
string fixed_real_path; // có thêm /root so với cái trên
string current_real_path;  // là (C:/root)
string current_fake_path; // là /root

WORD colorShell = FOREGROUND_GREEN | FOREGROUND_INTENSITY;   // mặc định
WORD colorCommand = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;  // mặc định (trắng)
string shellName = "my_shell";

void loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Config file not found, using default settings.\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Xóa comment
        size_t comment_pos = line.find_first_of(";#");
        if (comment_pos != std::string::npos)
            line = line.substr(0, comment_pos);

        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            // Trim khoảng trắng
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            try {
                if (key == "colorShell") {
                    colorShell = (WORD)std::stoi(value);
                } else if (key == "colorCommand") {
                    colorCommand = (WORD)std::stoi(value);
                } else if (key == "name") {
                    shellName = value;
                }
            } catch (...) {
                std::cerr << "Invalid config value for: " << key << "\n";
            }
        }
    }

    file.close();
}

void saveConfig(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Cannot save config file.\n";
        return;
    }
    file << "colorShell=" << (int)colorShell << "\n";
    file << "colorCommand=" << (int)colorCommand << "\n";
}



int shell_changeName(std::vector<std::string> args) {
    if (args.size() != 2) {
        std::cout << "Usage: name [new_name]\n";
        return 1;
    }

    shellName = args[1];
    std::cout << "Shell name changed to: " << shellName << "\n";

    // Ghi vào file config để lưu lại
    std::ofstream configFile("config\\.myshell_config");
    if (configFile.is_open()) {
        configFile << "colorShell=" << colorShell << "\n";
        configFile << "colorCommand=" << colorCommand << "\n";
        configFile << "name=" << shellName << "\n";
        configFile.close();
    } else {
        std::cerr << "Failed to write config file.\n";
    }

    return 0;
}



// Xử lý hàng đợi
HANDLE hJob;
JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli;

// foreground mode
HANDLE fore = NULL;



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
	"editor",
	"tree",
	"color",
	"name",
    "in"
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
	&shell_editor,
	&shell_tree,
	&shell_change_color,
	&shell_changeName,
    &shell_in
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

    loadConfig("config\\.myshell_config");

    string line;
    vector<string> args;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    while (status) {
        setTextColor(colorShell);  // màu shell prompt
        std::cout << shellName << ":";

        setTextColor(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        std::cout << formatFakePathToUnixStyle(current_fake_path);

        setTextColor(colorCommand);  // màu command text
        std::cout << "$ ";

        line = read_command_line();
        args = parse_command(line);
        args = formatToRealPath(args);
        if (!args.empty()) {
            shell_working(args);
        }
    }

    return 0;
}
