// interpreter.cpp
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "utils.h"
#include "flow_control.h"
#include "variable_manager.h"
#include "system_commands.h"
#include "script_io.h"

using namespace std;

map<string, int> g_label_map;

vector<string> builtin_str_script = {
    "goto",
    "if",
    "for",
    "while",
    "do",
    "exit"
};

int (*builtin_func_script[])(vector<string>) = {
    &shell_goto,
    &shell_if,
    &shell_for,
    &shell_while,
    &shell_do,
    &shell_exit
};

// Tìm index lệnh trong builtin_str_script (chỉ phục vụ trong bat)
int find_builtin_script(const string& cmd) {
    for (int i = 0; i < builtin_str_script.size(); ++i) {
        if (builtin_str_script[i] == cmd)
            return i;
    }
    return -1;
}

// Hàm chính chạy script
int shell_run_script(vector<string> args) {
    if (args.size() < 2) {
        cerr << "Usage: run_script <script_path>" << endl;
        return -1;
    }

    const string& filepath = current_real_path + "\\" + args[1];
    vector<string> script_lines;


    if (!read_script_file(filepath, script_lines, g_label_map)) {
        cerr << "Failed to load script file: " << filepath << endl;
        return -1;
    }

    int current_line = 0;
    int n = static_cast<int>(script_lines.size());

    while (current_line < n) {
        const string& line = script_lines[current_line];

        vector<string> cmd_args = parse_command(line);
        if (cmd_args.empty()) {
            ++current_line;
            continue;
        }

        string cmd = cmd_args[0];

        int idx = find_builtin(cmd);
        if (idx != -1) {
            int ret = builtin_func[idx](cmd_args);
            if (ret != 0) {
                cerr << "Error executing command '" << cmd << "' at line " << current_line + 1 << endl;
                return ret;
            }
            ++current_line;
            continue;
        }

        int script_idx = find_builtin_script(cmd);
        if (script_idx != -1) {
            int ret = builtin_func_script[script_idx](cmd_args);

            if (ret == -1) {
                cerr << "Error in control flow command '" << cmd << "' at line " << current_line + 1 << endl;
                return ret;
            }

            if (cmd == "exit") {
                return 0;
            }
            
            /*
            Phục vụ goto
            */
            if (ret >= 0 && ret < n) {
                current_line = ret;
                continue;
            }

            ++current_line;
            continue;
        }

        // Nếu không tìm thấy lệnh nào phù hợp
        cout << "Unknown command: " << cmd << endl;
        ++current_line;
    }

    return 0;
}