#include "flow_control.h"
#include "condition_evaluator.h"
#include "utils.h"
#include "interpreter.h"
#include "system_commands.h"


#include <iostream>
#include <map>


using namespace std;

map<string, int> label_map;

// if ... else ...
int shell_if(vector<string> args) {
    if (args.size() < 4 || args[1] != "(") {
        cerr << "Syntax error: if (condition) { command }" << endl;
        return -1;
    }

    // Ghép điều kiện giữa dấu ngoặc ()
    string condition;
    int i = 2;
    while (i < args.size() && args[i] != ")") {
        condition += args[i++] + " ";
    }

    if (i == args.size() || args[i] != ")") {
        cerr << "Missing ')' in condition" << endl;
        return -1;
    }

    // Ghép phần body sau dấu )
    string body;
    ++i;
    while (i < args.size()) {
        body += args[i++] + " ";
    }

    if (evaluate_condition(trim(condition))) {
        vector<string> body_args = parse_command(trim(body));
        if (body_args.empty()) return 0;

        string cmd = body_args[0];
        int idx = find_builtin(cmd);
        if (idx != -1) return builtin_func[idx](body_args);

        int script_idx = find_builtin_script(cmd);
        if (script_idx != -1) return builtin_func_script[script_idx](body_args);

        cerr << "Unknown command in if-body: " << cmd << endl;
        return -1;
    }

    return 0;
}


// goto :label
int shell_goto(vector<string> args) {
    if (args.size() < 2) {
        cerr << "goto: missing label argument" << endl;
        return -1;
    }

    string label = args[1];
    // xóa dấu ':' đầu nếu có
    if (!label.empty() && label[0] == ':') {
        label = label.substr(1);
    }

    auto it = g_label_map.find(label);
    if (it == g_label_map.end()) {
        cerr << "goto: label '" << label << "' not found" << endl;
        return -1;
    }

    // trả về dòng trong script để shell_run_script nhảy tới
    return it->second;
}

// build label->line mapping   --> kieu goto (set label)
void set_label_map(const vector<string> &script_lines) {
    
}

// for %%i in (...) do ...
int shell_for(vector<string> args) {
    return 0;
}

// while ... do (mở rộng)
int shell_while(vector<string> args) {
    return 0;    
}

// do ... while (mở rộng)
int shell_do_while(vector<string> args) {
    return 0;
}


int shell_do(vector<string> args) {
    return 0;
}

int shell_break_script(vector<string> args) {
    return 0;
}