#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include "utils.h"

using namespace std;

// Giả sử bạn đã có hàm trim() trong utils.h
string trim(const string& s);

// Hàm kiểm tra comment hoặc dòng rỗng
bool is_comment_or_empty(const string& line) {
    string trimmed = trim(line);
    if (trimmed.empty()) return true;

    string low = trimmed;
    transform(low.begin(), low.end(), low.begin(), ::tolower);

    if (low.compare(0, 3, "rem") == 0) return true;
    if (trimmed.compare(0, 2, "::") == 0) return true;

    return false;
}

/**
 * Đọc file .bat script
 * @param filepath : đường dẫn file
 * @param script_lines : vector để lưu các dòng lệnh (đã chuẩn hóa, loại bỏ comment)
 * @param label_map : map lưu label -> dòng số trong script_lines
 * @return true nếu đọc thành công
 */
bool read_script_file(const string& filepath, vector<string>& script_lines, map<string, int>& label_map) {
    ifstream fin(filepath);
    if (!fin.is_open()) {
        cerr << "Unable to open script file: " << filepath << endl;
        return false;
    }

    string line;
    int line_number = 0;
    while (getline(fin, line)) {
        // Loại bỏ ký tự \r (Windows \r\n)
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());

        // Bỏ qua comment và dòng rỗng
        if (is_comment_or_empty(line)) continue;

        // Chuẩn hóa dòng lệnh (trim)
        string clean_line = trim(line);

        if (clean_line.empty()) continue;

        // Nếu dòng bắt đầu bằng dấu :, đó là label
        if (clean_line[0] == ':') {
            string label_name = clean_line.substr(1);
            label_map[label_name] = line_number; 
            // Không thêm label vào script_lines (để chạy script không gặp lệnh này)
            // Nếu bạn muốn giữ lại label trong script_lines thì bỏ comment dòng bên dưới
            // script_lines.push_back(clean_line);
        } else {
            // Dòng bình thường, lưu vào vector script
            script_lines.push_back(clean_line);
            line_number++;
        }
    }

    fin.close();
    return true;
}

bool read_script_text_file(const string& filepath, vector<string>& lines) {
    ifstream file(current_real_path + "\\" + filepath);
    if (!file.is_open()) {
        cerr << "Cannot open script file: " << filepath << endl;
        return false;
    }

    string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    file.close();
    return true;
}