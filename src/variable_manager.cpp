#include "variable_manager.h"
#include <fstream>
#include <regex>
#include <iostream>

unordered_map<string, string> session_vars;    // Biến tạm thời trong phiên làm việc
unordered_map<string, string> persistent_vars; // Biến lưu trong sandbox (file)
string env_filename; // Đường dẫn file biến persistent sandbox

/*
Nhìn chung vẫn phải code lại cái này, không không kiểm soát được các biến vĩnh viễn, cũng
như thao tác trực tiếp với setx khá nguy hiểm
*/
/*
Lằng nhằng vc, tụ dưng lòi cái sandbox :)
*/

int shell_set(vector<string> args) {
    // Nếu chỉ gọi `set`, hiển thị tất cả biến session và persistent
    if (args.size() == 1) {
        auto vars = get_all_variables();
        for (const auto& [k, v] : vars) {
            cout << k << "=" << v << endl;
        }
        return 0;
    }

    // Nối các args từ 1 trở đi thành một chuỗi
    string combined;
    for (size_t i = 1; i < args.size(); ++i) {
        if (i > 1) combined += " ";
        combined += args[i];
    }

    size_t pos = combined.find('=');
    if (pos == string::npos) {
        cout << "Invalid syntax. Use var=value" << endl;
        return 1;
    }

    string var_name = combined.substr(0, pos);
    string var_value = combined.substr(pos + 1);

    set_variable(var_name, var_value, false);

    return 0;
}


int shell_setx(vector<string> args) {
    // Nếu chỉ gọi `setx`, hiển thị tất cả biến persistent
    if (args.size() == 1) {
        for (const auto& [k, v] : persistent_vars) {
            cout << k << "=" << v << endl;
        }
        return 0;
    }

    // Gộp các tham số từ 1 trở đi thành 1 chuỗi "x = 5"
    string combined;
    for (size_t i = 1; i < args.size(); ++i) {
        if (i > 1) combined += " ";
        combined += args[i];
    }

    size_t pos = combined.find('=');
    if (pos == string::npos) {
        cout << "Invalid syntax. Use var=value" << endl;
        return 1;
    }

    string var_name = combined.substr(0, pos);
    string var_value = combined.substr(pos + 1);

    set_variable(var_name, var_value, true);  // true -> lưu persistent
    save_persistent_vars();                   // lưu xuống file luôn

    return 0;
}

int shell_echo(vector<string> args) {
    if (args.size() < 2) {
        cout << endl;  // echo không có tham số thì xuống dòng
        return 0;
    }

    string output;
    for (size_t i = 1; i < args.size(); ++i) {
        if (i > 1) output += " ";
        output += resolve_variable(args[i]);  // phân giải biến ở đây
    }

    cout << output << endl;
    return 0;
}

bool init_variable_manager(const string& sandbox_env_file) {
    env_filename = sandbox_env_file;
    session_vars.clear();
    persistent_vars.clear();

    ifstream fin(env_filename);
    if (!fin.is_open()) {
        return true;
    }
    string line;
    while (getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto pos = line.find('=');
        if (pos == string::npos) continue;
        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);
        persistent_vars[key] = value;
    }
    fin.close();
    return true;
}

bool save_persistent_vars() {
    ofstream fout(env_filename, ios::trunc);
    if (!fout.is_open()) {
        cerr << "Error: Cannot write sandbox env file " << env_filename << endl;
        return false;
    }
    for (auto &kv : persistent_vars) {
        fout << kv.first << "=" << kv.second << "\n";
    }
    fout.close();
    return true;
}

string get_variable(const string &key) {
    auto it = session_vars.find(key);
    if (it != session_vars.end()) return it->second;
    auto it2 = persistent_vars.find(key);
    if (it2 != persistent_vars.end()) return it2->second;
    return "";
}

void set_variable(const string &key, const string &value, bool persistent) {
    if (persistent) {
        persistent_vars[key] = value;
        save_persistent_vars();
    } else {
        session_vars[key] = value;
    }
}

void unset_variable(const string &key) {
    session_vars.erase(key);
    persistent_vars.erase(key);
    save_persistent_vars();
}

string resolve_variable(const string &raw) {
    string result = raw;
    regex var_pattern("%([^%]+)%");
    size_t search_pos = 0;
    smatch match;

    while (search_pos < result.size()) {
        string tail = result.substr(search_pos);
        if (regex_search(tail, match, var_pattern)) {
            string full_match = match[0];
            string var_name = match[1];
            string var_value = get_variable(var_name);

            size_t pos = search_pos + match.position(0);
            result.replace(pos, full_match.length(), var_value);

            search_pos = pos + var_value.length();
        } else {
            break;
        }
    }

    return result;
}

unordered_map<string, string> get_all_variables() {
    unordered_map<string, string> all_vars = persistent_vars;
    // session vars override persistent
    for (auto &kv : session_vars) {
        all_vars[kv.first] = kv.second;
    }
    return all_vars;
}