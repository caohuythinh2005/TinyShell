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

vector<string> builtin_str_script;
int (*builtin_func_script[])(vector<string>);

// Tìm index lệnh trong builtin_str_script (chỉ phục vụ trong bat)
int find_builtin(const string& cmd) {
    for (int i = 0; i < (int)builtin_str.size(); ++i) {
        if (builtin_str[i] == cmd) return i;
    }
    return -1;
}

// Hàm chính chạy script
int shell_run_script(vector<string> args) {
    if (args.size() < 2) {
        cerr << "Usage: run_script <script_path>" << endl;
        return -1;
    }

    const string& filepath = args[1];
    vector<string> script_lines;
    map<string, int> label_map;

    if (!read_script_file(filepath, script_lines, label_map)) {
        cerr << "Failed to load script file: " << filepath << endl;
        return -1;
    }

    int current_line = 0;
    int n = (int)script_lines.size();

    while (current_line < n) {
        const string& line = script_lines[current_line];

        vector<string> cmd_args = parse_command(line);
        if (cmd_args.empty()) {
            ++current_line;
            continue;
        }

        string cmd = cmd_args[0];
        transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        int idx = find_builtin(cmd);
        if (idx != -1) {
            int ret = builtin_func[idx](cmd_args);

            // Lệnh điều khiển có thể trả về dòng cần nhảy tới
            if (cmd == "goto" || cmd == "if" || cmd == "for" || cmd == "while" || cmd == "do") {
                if (ret >= 0 && ret < n) {
                    current_line = ret;
                    continue;
                } else if (ret == -1) {
                    cerr << "Error in control flow at line " << current_line + 1 << endl;
                    return ret;
                }
            } else if (cmd == "exit") {
                return 0;
            } else {
                if (ret != 0) {
                    cerr << "Error executing command '" << cmd << "' at line " << current_line + 1 << endl;
                    return ret;
                }
            }
        } else {
            cout << "Unknown command: " << cmd << endl;
        }

        ++current_line;
    }

    return 0;
}