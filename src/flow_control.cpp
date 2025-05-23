#include "flow_control.h"
#include "condition_evaluator.h"
#include "utils.h"


#include <iostream>
#include <map>


using namespace std;

map<string, int> label_map;

// if ... else ...
int shell_if(vector<string> args) {
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