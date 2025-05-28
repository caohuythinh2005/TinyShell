#include "variables/variable_manager.h"
#include <fstream>
#include <regex>
#include <iostream>
#include "utilities/utils.h"
#include <unordered_map>
#include <unordered_set>
#include <stack>

unordered_map<string, string> session_vars;    // Biến tạm thời trong phiên làm việc
unordered_map<string, string> persistent_vars; // Biến lưu trong sandbox (file)
string env_filename; // Đường dẫn file biến persistent sandbox

/*
Nhìn chung vẫn phải code lại cái này, không không kiểm soát được các biến vĩnh viễn, cũng
như thao tác trực tiếp với setx khá nguy hiểm
*/
bool is_num(const string& s) {
    if (s.empty()) return false;
    if (s[0] == '-') return all_of(s.begin() + 1, s.end(), ::isdigit);
    return all_of(s.begin(), s.end(), ::isdigit);
}

vector<string> tk(const string& expr) {
    vector<string> res;
    string t;
    for (size_t i = 0; i < expr.length(); ++i) {
        char c = expr[i];
        if (isspace(c)) continue;

        if (isalnum(c) || c == '_') {
            t += c;
        } else {
            if (!t.empty()) {
                res.push_back(t);
                t.clear();
            }

            if (i + 1 < expr.length()) {
                string two = expr.substr(i, 2);
                if (two == "==" || two == "!=" || two == ">=" || two == "<=" ||
                    two == "&&" || two == "||") {
                    res.push_back(two);
                    ++i;
                    continue;
                }
            }

            res.emplace_back(1, c);
        }
    }
    if (!t.empty()) res.push_back(t);
    return res;
}

void replace_var(vector<string>& tokens) {
    for (auto& t : tokens) {
        if (!is_op(t) && !is_num(t) && t != "(" && t != ")") {
            if (session_vars.count(t)) {
                t = session_vars[t];
            } else {
                t = "0";
            }
        }
    }
}

int prec(const string& op) {
    if (op == "||") return 1;
    if (op == "&&") return 2;
    if (op == "==" || op == "!=") return 3;
    if (op == ">" || op == "<" || op == ">=" || op == "<=") return 4;
    if (op == "+" || op == "-") return 5;
    if (op == "*" || op == "/") return 6;
    return 0;
}

bool is_op(const string& token) {
    static const unordered_set<string> ops = {
        "+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">=", "&&", "||"
    };
    return ops.count(token);
}

vector<string> infix2postfix(const vector<string>& tokens) {
    vector<string> out;
    stack<string> st;

    for (const auto& token : tokens) {
        if (is_num(token)) {
            out.push_back(token);
        } else if (!is_op(token) && token != "(" && token != ")") {
            out.push_back(token);
        } else if (token == "(") {
            st.push(token);
        } else if (token == ")") {
            while (!st.empty() && st.top() != "(") {
                out.push_back(st.top());
                st.pop();
            }
            if (!st.empty()) st.pop();
        } else {
            while (!st.empty() && prec(st.top()) >= prec(token)) {
                out.push_back(st.top());
                st.pop();
            }
            st.push(token);
        }
    }
    while (!st.empty()) {
        out.push_back(st.top());
        st.pop();
    }
    return out;
}

int eval_postfix_expr(const vector<string>& postfix) {
    stack<int> stk;
    for (const auto& token : postfix) {
        if (is_num(token)) {
            stk.push(stoi(token));
        } else if (is_op(token)) {
            int b = stk.top(); stk.pop();
            int a = stk.top(); stk.pop();
            if (token == "+") stk.push(a + b);
            else if (token == "-") stk.push(a - b);
            else if (token == "*") stk.push(a * b);
            else if (token == "/") stk.push(b != 0 ? a / b : 0);
            else if (token == "==") stk.push(a == b);
            else if (token == "!=") stk.push(a != b);
            else if (token == "<") stk.push(a < b);
            else if (token == ">") stk.push(a > b);
            else if (token == "<=") stk.push(a <= b);
            else if (token == ">=") stk.push(a >= b);
            else if (token == "&&") stk.push(a && b);
            else if (token == "||") stk.push(a || b);
        }
    }
    return stk.top();
}

void evaluate_assignment(const string& expr_raw) {
    string expr = trim(expr_raw);
    size_t eq = expr.find('=');
    if (eq == string::npos) {
        cerr << "Invalid expression (missing '='): " << expr << endl;
        return;
    }

    string lhs = trim(expr.substr(0, eq));
    string rhs = trim(expr.substr(eq + 1));

    if (lhs.empty() || rhs.empty()) {
        cerr << "Invalid expression: " << expr << endl;
        return;
    }

    vector<string> tokens = tk(rhs);
    replace_var(tokens);
    vector<string> postfix = infix2postfix(tokens);
    int result = eval_postfix_expr(postfix);  // đổi tên gọi hàm tại đây

    session_vars[lhs] = to_string(result);
}

/*
Windows that ko ho tro setx /a vi ly do an toan
tham chi setx x= cung ko ho tro

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

    // Hỗ trợ set /a <biểu thức số học>
    if (args[1] == "/a") {
        string expr;
        for (size_t i = 2; i < args.size(); ++i) {
            if (i > 2) expr += " ";
            expr += args[i];
        }
        evaluate_assignment(expr);  // Hàm tự xử lý tính toán + gán vào session_vars
        return 0;
    }

    // Gộp các args từ 1 trở đi thành 1 chuỗi "x = 5"
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

    string var_name = trim(combined.substr(0, pos));
    string var_value = trim(combined.substr(pos + 1));

    // Nếu giá trị rỗng, tức là unset (CMD-style)
    if (var_value.empty()) {
        unset_variable(var_name);
    } else {
        set_variable(var_name, var_value, false);  // false = session variable
    }

    return 0;
}

// int shell_set(vector<string> args) {
//     // Nếu chỉ gọi `set`, hiển thị tất cả biến session và persistent
//     if (args.size() == 1) {
//         auto vars = get_all_variables();
//         for (const auto& [k, v] : vars) {
//             cout << k << "=" << v << endl;
//         }
//         return 0;
//     }

//     // Gộp các args từ 1 trở đi thành 1 chuỗi "x = 5"
//     string combined;
//     for (size_t i = 1; i < args.size(); ++i) {
//         if (i > 1) combined += " ";
//         combined += args[i];
//     }

//     size_t pos = combined.find('=');
//     if (pos == string::npos) {
//         cout << "Invalid syntax. Use var=value" << endl;
//         return 1;
//     }

//     string var_name = combined.substr(0, pos);
//     string var_value = combined.substr(pos + 1);

//     // Nếu giá trị rỗng, tức là unset (CMD-style)
//     if (var_value.empty()) {
//         unset_variable(var_name);
//     } else {
//         set_variable(var_name, var_value, false);
//     }

//     return 0;
// }



int shell_setx(vector<string> args) {
    if (args.size() == 1) {
        for (const auto& [k, v] : persistent_vars) {
            cout << k << "=" << v << endl;
        }
        return 0;
    }
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

    // Nếu giá trị rỗng → không cho phép unset persistent
    // CMD thực chất cũng thế
    if (var_value.empty()) {
        cout << "Error: Cannot unset persistent variable using setx. Use set instead." << endl;
        return 1;
    }

    // Cập nhật cả hai nơi
    persistent_vars[var_name] = var_value;
    session_vars[var_name] = var_value;

    save_persistent_vars();

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

    // Sau khi load xong thì nạp toàn bộ vào session
    session_vars = persistent_vars;

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
    // KHÔNG xóa khỏi persistent_vars ở đây
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
    /*
    Chỉ trả về các biến trong session.
    Mặc định các biến persistent đã được nạp vào session từ init rồi.
    */
    return session_vars;
}