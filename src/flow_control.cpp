#include "flow_control.h"
#include "condition_evaluator.h"
#include "utils.h"


#include <iostream>
#include <map>


using namespace std;

map<string, int> label_map;

// if ... else ...
int shell_if(vector<string> args) {
    // args: [ "if", <condition parts...>, <command parts...> ]

    if (args.size() < 3) {
        cerr << "if: too few arguments" << endl;
        return -1;
    }

    // 1. Phân tích điều kiện
    int cond_start = 1;
    bool negate = false;

    if (args[cond_start] == "not") {
        negate = true;
        cond_start++;
    }

    // Ví dụ: args[cond_start] có thể là 'exist', 'errorlevel', 'string compare', etc.
    // Bạn cần viết code riêng cho từng kiểu điều kiện.

    bool cond_result = false;
    int cond_end = -1; // index cuối cùng của điều kiện, để biết chỗ bắt đầu command

    // Ví dụ xử lý điều kiện 'exist'
    if (args[cond_start] == "exist" && cond_start + 1 < (int)args.size()) {
        string file = args[cond_start + 1];
        cond_result = file_exists(file);
        cond_end = cond_start + 1;
    } 
    else if (args[cond_start] == "errorlevel" && cond_start + 1 < (int)args.size()) {
        int level = stoi(args[cond_start + 1]);
        cond_result = (g_last_errorlevel >= level);  // giả sử có biến lưu errorlevel
        cond_end = cond_start + 1;
    }
    else {
        // Có thể xử lý thêm các kiểu so sánh chuỗi/numeric
        // Nếu không hiểu điều kiện thì báo lỗi
        cerr << "if: unsupported condition" << endl;
        return -1;
    }

    if (negate) cond_result = !cond_result;

    // 2. Nếu điều kiện sai thì không làm gì, trả về 0
    if (!cond_result) {
        return 0;
    }

    // 3. Nếu đúng thì chạy lệnh bắt đầu từ cond_end + 1
    if (cond_end + 1 >= (int)args.size()) return 0;

    vector<string> cmd_args(args.begin() + cond_end + 1, args.end());

    if (cmd_args.empty()) return 0;

    string cmd = cmd_args[0];

    // Tìm lệnh builtin
    int idx = find_builtin(cmd);
    if (idx != -1) {
        return builtin_func[idx](cmd_args);
    }

    // Tìm lệnh control flow hoặc script
    int script_idx = find_builtin_script(cmd);
    if (script_idx != -1) {
        return builtin_func_script[script_idx](cmd_args);
    }

    cerr << "if: unknown command '" << cmd << "'" << endl;
    return -1;
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