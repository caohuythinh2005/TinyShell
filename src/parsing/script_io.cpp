#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include "utilities/utils.h"

using namespace std;


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
 * @return true nếu đọc thành công
 */
bool read_script_file(const string& filepath, vector<string>& script_lines) {
    ifstream fin(filepath);
    if (!fin.is_open()) {
        cerr << "Unable to open script file: " << filepath << endl;
        return false;
    }

    string line;
    while (getline(fin, line)) {
        // Loại bỏ ký tự \r (Windows \r\n)
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());

        // Bỏ qua comment và dòng rỗng
        if (is_comment_or_empty(line)) continue;

        // Chuẩn hóa dòng lệnh (trim)
        string clean_line = trim(line);

        if (clean_line.empty()) continue;

        // Bỏ qua dòng label (bắt đầu bằng ':')
        if (clean_line[0] == ':') {
            // Không thêm vào script_lines, bỏ qua
            continue;
        }

        // Lưu dòng lệnh đã chuẩn hóa
        script_lines.push_back(clean_line);
    }

    fin.close();
    return true;
}