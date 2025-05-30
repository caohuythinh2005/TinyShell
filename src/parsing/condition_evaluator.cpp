#include <stack>
#include <string>
#include <vector>
#include <iostream>
#include <cctype>
#include <algorithm>

#include "utilities/globals.h"
#include "parsing/condition_evaluator.h"
#include "utilities/utils.h"
#include "variables/variable_manager.h"
#include "filesystem/directory_manager.h"
#include "utilities/constant.h"

using namespace std;


int shell_eval(vector<string> args) {
    string command = "";
    for (int i = 1; i < args.size(); i++) {
        command += (" " + args[i]);
    }
    cout << evaluate_condition(command) << endl;
    return 0;
}

string strip_outer_parentheses(const string& expr) {
    string s = trim(expr);
    while (!s.empty() && s.front() == '(' && s.back() == ')') {
        int count = 0;
        bool balanced = false;
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '(') count++;
            else if (s[i] == ')') count--;
            if (count == 0 && i != s.size() - 1) {
                balanced = false;
                break;
            }
        }
        if (count == 0) balanced = true;

        if (balanced) {
            s = trim(s.substr(1, s.size() - 2));
        } else {
            break;
        }
    }
    return s;
}

bool is_number(const string& s) {
    if (s.empty()) return false;
    if (s[0] == '-') return all_of(s.begin() + 1, s.end(), ::isdigit);
    return all_of(s.begin(), s.end(), ::isdigit);
}

vector<string> tokenize(const string& expr) {
    string trimmed = trim(expr);
    vector<string> tokens;

    string token;
    for (size_t i = 0; i < trimmed.length(); ++i) {
        char c = trimmed[i];
        if (isspace(c)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            continue;
        }

        if (isalnum(c) || c == '_' || c == '.' || c == '/' || c == '\\' || c == ':' || c == '-') {
            token += c;
        } else {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            // Xử lý các toán tử 2 ký tự
            if (i + 1 < trimmed.size()) {
                string two_chars = trimmed.substr(i, 2);
                if (two_chars == "==" || two_chars == "!=" || two_chars == ">=" || two_chars == "<=" ||
                    two_chars == "&&" || two_chars == "||") {
                    tokens.push_back(two_chars);
                    ++i;
                    continue;
                }
            }
            // Toán tử 1 ký tự
            tokens.emplace_back(1, c);
        }
    }
    if (!token.empty()) tokens.push_back(token);

    return tokens;
}

void replace_variables(vector<string>& tokens) {
    for (auto& t : tokens) {
        if (session_vars.count(t)) {
            t = session_vars[t];
        }
    }
}

int precedence(const string& op) {
    if (op == "||") return 1;
    if (op == "&&") return 2;
    if (op == "==") return 3;
    if (op == "!=") return 3;
    if (op == ">") return 4;
    if (op == "<") return 4;
    if (op == ">=") return 4;
    if (op == "<=") return 4;
    if (op == "+") return 5;
    if (op == "-") return 5;
    if (op == "*") return 6;
    if (op == "/") return 6;
    if (op == "!") return 7; // unary NOT có precedence cao nhất
    return 0;
}

bool is_operator(const string& token) {
    static vector<string> ops = {
        "+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!"
    };
    return find(ops.begin(), ops.end(), token) != ops.end();
}

vector<string> infix_to_postfix(const vector<string>& tokens) {
    vector<string> output;
    stack<string> ops;

    for (size_t i = 0; i < tokens.size(); ++i) {
        const string& token = tokens[i];
        if (is_number(token)) {
            output.push_back(token);
        } else if (!is_operator(token) && token != "(" && token != ")") {
            output.push_back(token);
        } else if (token == "(") {
            ops.push(token);
        } else if (token == ")") {
            while (!ops.empty() && ops.top() != "(") {
                output.push_back(ops.top());
                ops.pop();
            }
            if (!ops.empty()) ops.pop();
        } else {
            // Toán tử đơn (unary) như "!" cần xử lý đặc biệt
            while (!ops.empty() && precedence(ops.top()) >= precedence(token)) {
                if (token == "!" && ops.top() == "!") break; // tránh vòng lặp vô hạn cho unary
                output.push_back(ops.top());
                ops.pop();
            }
            ops.push(token);
        }
    }
    while (!ops.empty()) {
        output.push_back(ops.top());
        ops.pop();
    }
    return output;
}

bool evaluate_postfix(const vector<string>& postfix) {
    stack<int> stk;
    for (const auto& token : postfix) {
        if (is_number(token)) {
            stk.push(stoi(token));
        } else if (is_operator(token)) {
            if (token == "!") {
                if (stk.empty()) return false;
                int a = stk.top(); stk.pop();
                stk.push(!a);
            } else {
                if (stk.size() < 2) return false;
                int b = stk.top(); stk.pop();
                int a = stk.top(); stk.pop();

                if (token == "+") stk.push(a + b);
                else if (token == "-") stk.push(a - b);
                else if (token == "*") stk.push(a * b);
                else if (token == "/") stk.push(a / b);
                else if (token == "==") stk.push(a == b);
                else if (token == "!=") stk.push(a != b);
                else if (token == "<") stk.push(a < b);
                else if (token == ">") stk.push(a > b);
                else if (token == "<=") stk.push(a <= b);
                else if (token == ">=") stk.push(a >= b);
                else if (token == "&&") stk.push(a && b);
                else if (token == "||") stk.push(a || b);
            }
        } else {
            // Nếu gặp token không rõ, đẩy 0 (false)
            stk.push(0);
        }
    }
    return !stk.empty() && stk.top();
}

// Hàm tiền xử lý: thay thế exist/notexist <path> thành 1 hoặc 0
string preprocess_exist_conditions(const string& expr) {
    vector<string> tokens = tokenize(expr);
    vector<string> new_tokens;

    for (size_t i = 0; i < tokens.size();) {
        if (tokens[i] == "(" && (i + 1 < tokens.size()) &&
            (tokens[i+1] == "exist" || tokens[i+1] == "notexist") && i + 2 < tokens.size()) {
            // Trường hợp (exist apps)
            string cmd = tokens[i+1];
            string path_str = tokens[i+2];
            string full_path;

            if ((path_str.size() >= 5) && (path_str.substr(0, 5) == "/root" || path_str.substr(0, 5) == "\\root")) {
                full_path = path_str;
            } else {
                full_path = current_fake_path + "\\" + path_str;
            }
            bool exists = (fileExists(full_path) == EXIST_FILE_OR_DIRECTORY) || (folderExists(full_path) == EXIST_FILE_OR_DIRECTORY);
            int val = (cmd == "exist") ? (exists ? 1 : 0) : (exists ? 0 : 1);

            new_tokens.push_back(to_string(val));
            i += 4; // Bỏ qua '(' + cmd + path + ')'
        } else if ((tokens[i] == "exist" || tokens[i] == "notexist") && i + 1 < tokens.size()) {
            // Trường hợp exist apps không có ngoặc
            string cmd = tokens[i];
            string path_str = tokens[i + 1];
            string full_path;

            if ((path_str.size() >= 5) && (path_str.substr(0, 5) == "/root" || path_str.substr(0, 5) == "\\root")) {
                full_path = path_str;
            } else {
                full_path = current_fake_path + "\\" + path_str;
            }
            bool exists = (fileExists(full_path) == EXIST_FILE_OR_DIRECTORY) || (folderExists(full_path) == EXIST_FILE_OR_DIRECTORY);
            int val = (cmd == "exist") ? (exists ? 1 : 0) : (exists ? 0 : 1);

            new_tokens.push_back(to_string(val));
            i += 2;
        } else {
            new_tokens.push_back(tokens[i]);
            i++;
        }
    }
    // Ghép lại chuỗi
    string result = "";
    for (size_t i = 0; i < new_tokens.size(); ++i) {
        if (i > 0) result += " ";
        result += new_tokens[i];
    }
    return result;
}

bool evaluate_condition(const string& expr_raw) {
    string expr = strip_outer_parentheses(trim(expr_raw));

    // Tiền xử lý các điều kiện exist/notexist thành 1 hoặc 0
    string expr_processed = preprocess_exist_conditions(expr);

    vector<string> tokens = tokenize(expr_processed);
    replace_variables(tokens);

    vector<string> postfix = infix_to_postfix(tokens);
    return evaluate_postfix(postfix);
}
