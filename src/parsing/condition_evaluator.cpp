/*

Ông code phần này nhé

Code để nó có thể xử lý biểu thức phức tạp

void evaluate_assignment(const string& expr) lien quan cai nay nua

*/

#include <stack>

#include "utilities/globals.h"
#include "parsing/condition_evaluator.h"
#include "utilities/utils.h"
#include "variables/variable_manager.h"


int shell_eval(vector<string> args) {
    string command = "";
    for (int i = 1; i < args.size(); i++) {
        command += args[i];
    }
    cout << evaluate_condition(command) << endl;
    return 0;
}

bool is_number(const string& s) {
    if (s.empty()) return false;
    if (s[0] == '-') return all_of(s.begin() + 1, s.end(), ::isdigit);
    return all_of(s.begin(), s.end(), ::isdigit);
}

// ---------- Tokenize ----------

vector<string> tokenize(const string& expr) {
    vector<string> tokens;
    string token;
    for (size_t i = 0; i < expr.length(); ++i) {
        char c = expr[i];
        if (isspace(c)) continue;

        if (isdigit(c) || isalpha(c)) {
            token += c;
        } else {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }

            if (c == '=' && expr[i + 1] == '=') {
                tokens.push_back("=="); ++i;
            } else if (c == '!' && expr[i + 1] == '=') {
                tokens.push_back("!="); ++i;
            } else if (c == '>' && expr[i + 1] == '=') {
                tokens.push_back(">="); ++i;
            } else if (c == '<' && expr[i + 1] == '=') {
                tokens.push_back("<="); ++i;
            } else if (c == '&' && expr[i + 1] == '&') {
                tokens.push_back("&&"); ++i;
            } else if (c == '|' && expr[i + 1] == '|') {
                tokens.push_back("||"); ++i;
            } else {
                tokens.emplace_back(1, c);
            }
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

// ---------- Replace Variables ----------

void replace_variables(vector<string>& tokens) {
    for (auto& t : tokens) {
        if (session_vars.count(t)) {
            t = session_vars[t];
        }
    }
}

// ---------- Operator Precedence ----------

int precedence(const string& op) {
    if (op == "||") return 1;
    if (op == "&&") return 2;
    if (op == "==" || op == "!=") return 3;
    if (op == ">" || op == "<" || op == ">=" || op == "<=") return 4;
    if (op == "+" || op == "-") return 5;
    if (op == "*" || op == "/") return 6;
    return 0;
}

bool is_operator(const string& token) {
    static vector<string> ops = {"+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">=", "&&", "||"};
    return find(ops.begin(), ops.end(), token) != ops.end();
}

// ---------- Infix to Postfix ----------

vector<string> infix_to_postfix(const vector<string>& tokens) {
    vector<string> output;
    stack<string> ops;

    for (const auto& token : tokens) {
        if (is_number(token)) {
            output.push_back(token);
        } else if (!is_operator(token) && token != "(" && token != ")") {
            output.push_back(token); // variable or unknown, already replaced
        } else if (token == "(") {
            ops.push(token);
        } else if (token == ")") {
            while (!ops.empty() && ops.top() != "(") {
                output.push_back(ops.top()); ops.pop();
            }
            if (!ops.empty()) ops.pop(); // remove "("
        } else {
            while (!ops.empty() && precedence(ops.top()) >= precedence(token)) {
                output.push_back(ops.top()); ops.pop();
            }
            ops.push(token);
        }
    }
    while (!ops.empty()) {
        output.push_back(ops.top()); ops.pop();
    }
    return output;
}

// ---------- Postfix Evaluation ----------

bool evaluate_postfix(const vector<string>& postfix) {
    stack<int> stk;
    for (const auto& token : postfix) {
        if (is_number(token)) {
            stk.push(stoi(token));
        } else if (is_operator(token)) {
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
    }
    return stk.top();
}

// ---------- Main Evaluator ----------

bool evaluate_condition(const string& expr_raw) {
    string expr = trim(expr_raw);
    vector<string> tokens = tokenize(expr);
    replace_variables(tokens);
    vector<string> postfix = infix_to_postfix(tokens);
    return evaluate_postfix(postfix);
}