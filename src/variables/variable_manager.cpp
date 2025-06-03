#include "variables/variable_manager.h"
#include <fstream>
#include <regex>
#include <iostream>
#include "utilities/utils.h"
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include<filesystem/path_manager.h>
#include <limits> 
unordered_map<string, string> session_vars;    // Biến tạm thời trong phiên làm việc
unordered_map<string, string> persistent_vars; // Biến lưu trong sandbox (file)
string env_filename; // Đường dẫn file biến persistent sandbox

/*
Nhìn chung vẫn phải code lại cái này, không không kiểm soát được các biến vĩnh viễn, cũng
như thao tác trực tiếp với setx khá nguy hiểm
*/
// Kiểm tra xem string có phải số (có dấu âm hay không)
bool vm_is_num(const string& s) {
    if (s.empty()) return false;
    int start = (s[0] == '-') ? 1 : 0;
    return all_of(s.begin() + start, s.end(), ::isdigit);
}

// Kiểm tra xem token có phải toán tử không
bool vm_is_op(const string& token) {
    static const unordered_set<string> ops = {
        "+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">=", "&&", "||"
    };
    return ops.count(token);
}

// Độ ưu tiên toán tử
int vm_precedence(const string& op) {
    if (op == "||") return 1;
    if (op == "&&") return 2;
    if (op == "==" || op == "!=") return 3;
    if (op == ">" || op == "<" || op == ">=" || op == "<=") return 4;
    if (op == "+" || op == "-") return 5;
    if (op == "*" || op == "/") return 6;
    return 0;
}

// Tách chuỗi biểu thức thành các token (biến, số, toán tử, ngoặc)
vector<string> vm_tokenize(const string& expr) {
    vector<string> tokens;
    string temp;
    for (size_t i = 0; i < expr.length(); ++i) {
        char c = expr[i];
        if (isspace(c)) continue;

        if (isalnum(c) || c == '_') {
            temp += c;
        } else {
            if (!temp.empty()) {
                tokens.push_back(temp);
                temp.clear();
            }

            if (i + 1 < expr.length()) {
                string two = expr.substr(i, 2);
                if (vm_is_op(two)) {
                    tokens.push_back(two);
                    ++i;
                    continue;
                }
            }

            tokens.emplace_back(1, c);
        }
    }
    if (!temp.empty()) tokens.push_back(temp);
    return tokens;
}

// Thay thế biến trong token bằng giá trị tương ứng
void vm_replace_variables(vector<string>& tokens) {
    for (auto& tok : tokens) {
        if (!vm_is_op(tok) && !vm_is_num(tok) && tok != "(" && tok != ")") {
            if (session_vars.count(tok)) {
                tok = session_vars[tok];
            } else if (persistent_vars.count(tok)) {
                tok = persistent_vars[tok];
            } else {
                tok = "0";  // fallback
            }
        }
    }
}

// Chuyển biểu thức infix sang postfix
vector<string> vm_infix_to_postfix(const vector<string>& tokens) {
    vector<string> output;
    stack<string> op_stack;

    for (const auto& tok : tokens) {
        if (vm_is_num(tok)) {
            output.push_back(tok);
        } else if (tok == "(") {
            op_stack.push(tok);
        } else if (tok == ")") {
            while (!op_stack.empty() && op_stack.top() != "(") {
                output.push_back(op_stack.top());
                op_stack.pop();
            }
            if (!op_stack.empty()) op_stack.pop(); // pop '('
        } else if (vm_is_op(tok)) {
            while (!op_stack.empty() && vm_precedence(op_stack.top()) >= vm_precedence(tok)) {
                output.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.push(tok);
        } else {
            // unexpected token, treat as variable (đã replace trước đó)
            output.push_back(tok);
        }
    }

    while (!op_stack.empty()) {
        output.push_back(op_stack.top());
        op_stack.pop();
    }
    return output;
}

// Đánh giá biểu thức postfix
int vm_evaluate_postfix(const vector<string>& postfix) {
    stack<int> stk;
    for (const auto& tok : postfix) {
        if (vm_is_num(tok)) {
            stk.push(stoi(tok));
        } else if (vm_is_op(tok)) {
            if (stk.size() < 2) {
                cerr << "Invalid expression: insufficient operands for '" << tok << "'\n";
                return 0;
            }
            int b = stk.top(); stk.pop();
            int a = stk.top(); stk.pop();

            if (tok == "+") stk.push(a + b);
            else if (tok == "-") stk.push(a - b);
            else if (tok == "*") stk.push(a * b);
            else if (tok == "/") stk.push(b != 0 ? a / b : 0);
            else if (tok == "==") stk.push(a == b);
            else if (tok == "!=") stk.push(a != b);
            else if (tok == "<") stk.push(a < b);
            else if (tok == ">") stk.push(a > b);
            else if (tok == "<=") stk.push(a <= b);
            else if (tok == ">=") stk.push(a >= b);
            else if (tok == "&&") stk.push(a && b);
            else if (tok == "||") stk.push(a || b);
        } else {
            cerr << "Unknown token in evaluation: " << tok << endl;
            return 0;
        }
    }

    if (stk.size() != 1) {
        cerr << "Invalid postfix expression. Stack size: " << stk.size() << endl;
        return 0;
    }

    return stk.top();
}

// Hàm chính để xử lý gán biểu thức dạng a = expr
void evaluate_assignment(const string& expr_raw) {
    string expr = trim(expr_raw);

    // Tìm vị trí toán tử gán, có thể là =, +=, -=, *=, /=
    size_t pos = string::npos;
    string assign_op;

    vector<string> assign_ops = {"+=", "-=", "*=", "/=", "="};
    for (const auto& op : assign_ops) {
        size_t p = expr.find(op);
        if (p != string::npos) {
            if (pos == string::npos || p < pos) {
                pos = p;
                assign_op = op;
            }
        }
    }

    if (pos == string::npos) {
        cerr << "Error: Missing assignment operator in expression: " << expr << endl;
        return;
    }

    string lhs = trim(expr.substr(0, pos));
    string rhs = trim(expr.substr(pos + assign_op.length()));

    if (lhs.empty() || rhs.empty()) {
        cerr << "Error: Invalid assignment: " << expr << endl;
        return;
    }

    // Nếu biến lhs chưa có, khởi tạo = 0
    if (!session_vars.count(lhs) && !persistent_vars.count(lhs)) {
        session_vars[lhs] = "0";
    }

    vector<string> tokens = vm_tokenize(rhs);
    vm_replace_variables(tokens);
    vector<string> postfix = vm_infix_to_postfix(tokens);
    int rhs_val;

    try {
        rhs_val = vm_evaluate_postfix(postfix);
    } catch (const std::exception& e) {
        cerr << "Error evaluating right-hand side: " << endl;
        return;
    }

    int lhs_val = 0;
    try {
        if (session_vars.count(lhs)) lhs_val = stoi(session_vars[lhs]);
        else if (persistent_vars.count(lhs)) lhs_val = stoi(persistent_vars[lhs]);
    } catch (const std::exception& e) {
        cerr << "Error converting LHS variable '" << lhs << "' to integer: " << endl;
        return;
    }

    int result = 0;
    if (assign_op == "=") {
        result = rhs_val;
    } else if (assign_op == "+=") {
        result = lhs_val + rhs_val;
    } else if (assign_op == "-=") {
        result = lhs_val - rhs_val;
    } else if (assign_op == "*=") {
        result = lhs_val * rhs_val;
    } else if (assign_op == "/=") {
        if (rhs_val == 0) {
            cerr << "Error: Division by zero\n";
            return;
        }
        result = lhs_val / rhs_val;
    } else {
        cerr << "Error: Unsupported assignment operator: " << assign_op << endl;
        return;
    }

    session_vars[lhs] = to_string(result);
}



int shell_in(vector<string> args) {
    if (args.size() != 2) {
        cerr << "Usage: in VAR_NAME" << endl;
        return 0;
    }

    string var_name = args[1];
    string value;

    cout << "> ";
    getline(cin, value);  // Đọc cả dòng nhập vào

    set_variable(var_name, value);
    return 0;
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
            if (k == "PATH") {
                cout << "PATH=";
                for (auto s : envPaths) {
                    cout << formatFakePathToUnixStyle(s) << ";";
                }
                cout << "\n";
            } else {
                cout << k << "=" << v << endl;
            }
        }
        return 0;
    }

    // Hỗ trợ set /a <biểu thức số học>
    if (args[1] == "\\a") {
        string expr;
        for (size_t i = 2; i < args.size(); ++i) {
            if (i > 2) expr += " ";
            expr += args[i];
        }
        // Resolve biến kiểu %var% trong expr trước
        expr = resolve_variable(expr);

        evaluate_assignment(expr);
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

    string var_name_raw = trim(combined.substr(0, pos));
    string var_value_raw = trim(combined.substr(pos + 1));

    // Phân giải biến trong tên biến
    string var_name = resolve_variable(var_name_raw);

    // Phân giải biến trong giá trị
    string var_value = resolve_variable(var_value_raw);

    // Nếu giá trị rỗng, unset biến
    if (var_value.empty()) {
        unset_variable(var_name);
    } else {
        set_variable(var_name, var_value, false);  // false = session variable
    }

    if (var_name == "PATH") {
        envPaths.clear(); // xóa path cũ
        initPath();       // nạp lại path mới
    }

    return 0;
}

int shell_setx(vector<string> args) {
    if (args.size() > 1 && args[1] == "\\a") {
        cout << "Error: The /a option is not supported by setx." << endl;
        return 1;
    }
    if (args.size() == 1) {
        // for (const auto& [k, v] : persistent_vars) {
        //     cout << k << "=" << v << endl;
        // }
        cout << "ERROR: Invalid syntax.\n";
        cout << "Type 'SETX var=value'\n";
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
    if (var_name == "PATH") {
            envPaths.clear(); // xóa path cũ
            initPath();       // nạp lại path từ file mới
    }
    return 0;
}

int shell_echo(vector<string> args) {
    if (args.size() < 2) {
        cout << endl;
        return 0;
    }

    for (size_t i = 1; i < args.size(); ++i) {
        string resolved = resolve_variable(args[i]);
        cout << resolved << " ";
    }
    cout << endl;
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

// Hàm resolve biến dạng %...%, đệ quy để giải biến lồng nhau
string resolve_variable(const string& input) {
    string output;
    size_t pos = 0;
    while (pos < input.size()) {
        size_t start = input.find('%', pos);
        if (start == string::npos) {
            output += input.substr(pos);
            break;
        }
        output += input.substr(pos, start - pos);

        int depth = 0;
        size_t end = start + 1;
        for (; end < input.size(); ++end) {
            if (input[end] == '[') depth++;
            else if (input[end] == ']') depth--;
            else if (input[end] == '%' && depth == 0) break;
        }

        if (end == input.size()) {
            output += input.substr(start);
            break;
        }

        string inner = input.substr(start + 1, end - start - 1);
        string resolved_inner = resolve_variable(inner);
        string var_value = get_variable(resolved_inner);

        output += var_value;
        pos = end + 1;
    }
    return output;
}

unordered_map<string, string> get_all_variables() {
    /*
    Chỉ trả về các biến trong session.
    Mặc định các biến persistent đã được nạp vào session từ init rồi.
    */
    return session_vars;
}