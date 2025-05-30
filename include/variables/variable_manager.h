#ifndef _VARIABLE_MANAGER_H_
#define _VARIABLE_MANAGER_H_

#include <string>
#include <unordered_map>

#define _HAS_STD_BYTE 0

using namespace std;

/*
Các biến lưu trữ
*/

extern unordered_map<string, string> session_vars;    // Biến tạm thời trong phiên làm việc
extern unordered_map<string, string> persistent_vars; // Biến lưu trong sandbox (file)
extern string env_filename; // Đường dẫn file biến persistent sandbox

/*
Mục đích : hỗ tợ lưu trữ kiểu persistent và theo session -> có thể lưu path vĩnh viễn
*/
// bool is_number(const string& s);
// vector<string> tokenize(const string& expr);
// void replace_variables(vector<string>& tokens);
// int precedence(const string& op);
bool is_op(const string& token);
// vector<string> infix_to_postfix(const vector<string>& tokens);
// int evaluate_postfix(const vector<string>& postfix);
void evaluate_assignment(const string& expr);

int shell_set(vector<string> args);
int shell_setx(vector<string> args);
// để ở đây chưa đúng lắm, sửa lại sau
int shell_echo(vector<string> args);


// Khởi tạo và load biến persistent từ file sandbox
bool init_variable_manager(const string& sandbox_env_file);

// Lưu biến persistent vào file
bool save_persistent_vars();

// Lấy giá trị biến (ưu tiên session_vars, sau đó persistent_vars)
string get_variable(const string &key);

// Gán biến, persistent = true để lưu lâu dài sandbox
void set_variable(const string &key, const string &value, bool persistent = false);

// Xóa biến (cả session và persistent)
void unset_variable(const string &key);

// Giải mã chuỗi chứa biến dạng %var%
string resolve_variable(const string &raw);

// Lấy tất cả biến (session + persistent)
unordered_map<string, string> get_all_variables();



#endif
