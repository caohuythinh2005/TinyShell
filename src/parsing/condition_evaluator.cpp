/*

Ông code phần này nhé

Code để nó có thể xử lý biểu thức phức tạp

void evaluate_assignment(const string& expr) lien quan cai nay nua

*/

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

bool evaluate_condition(const string& expr_raw) {
    string expr = trim(expr_raw);

    // Bỏ dấu ngoặc ( )
    if (!expr.empty() && expr.front() == '(' && expr.back() == ')') {
        expr = expr.substr(1, expr.length() - 2);
    }

    // Các toán tử hỗ trợ
    vector<string> ops = {">=", "<=", "==", "!=", ">", "<"};
    string op;
    size_t pos = string::npos;

    for (const auto& o : ops) {
        pos = expr.find(o);
        if (pos != string::npos) {
            op = o;
            break;
        }
    }

    if (op.empty() || pos == string::npos) {
        cerr << "Invalid condition: " << expr_raw << endl;
        return false;
    }

    // Tách trái và phải
    string lhs = trim(expr.substr(0, pos));
    string rhs = trim(expr.substr(pos + op.length()));

    // Lấy giá trị từ session_vars hoặc giữ nguyên
    string lhs_val = session_vars.count(lhs) ? session_vars[lhs] : lhs;
    string rhs_val = session_vars.count(rhs) ? session_vars[rhs] : rhs;

    // So sánh số nếu có thể
    bool lhs_is_num = all_of(lhs_val.begin(), lhs_val.end(), ::isdigit) || 
                      (lhs_val[0] == '-' && lhs_val.size() > 1 && all_of(lhs_val.begin() + 1, lhs_val.end(), ::isdigit));
    bool rhs_is_num = all_of(rhs_val.begin(), rhs_val.end(), ::isdigit) || 
                      (rhs_val[0] == '-' && rhs_val.size() > 1 && all_of(rhs_val.begin() + 1, rhs_val.end(), ::isdigit));

    if (lhs_is_num && rhs_is_num) {
        int left = stoi(lhs_val);
        int right = stoi(rhs_val);

        if (op == "==") return left == right;
        if (op == "!=") return left != right;
        if (op == ">")  return left > right;
        if (op == "<")  return left < right;
        if (op == ">=") return left >= right;
        if (op == "<=") return left <= right;
    } else {
        // So sánh chuỗi nếu không phải số
        if (op == "==") return lhs_val == rhs_val;
        if (op == "!=") return lhs_val != rhs_val;
        if (op == ">")  return lhs_val > rhs_val;
        if (op == "<")  return lhs_val < rhs_val;
        if (op == ">=") return lhs_val >= rhs_val;
        if (op == "<=") return lhs_val <= rhs_val;
    }

    return false;
}