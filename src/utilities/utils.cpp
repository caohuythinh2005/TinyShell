#include "utilities/utils.h"
#include "utilities/constant.h"
#include <string>
#include <cctype>
#include <map>

using namespace std;


// Cắt khoảng trắng đầu và cuối chuỗi (trim)
// C++ ko có (??)
string trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    if (start == s.size()) return "";

    size_t end = s.size() - 1;
    while (end > start && isspace(static_cast<unsigned char>(s[end]))) {
        --end;
    }
    return s.substr(start, end - start + 1);
}


string removePrefix(const string& full, const string& prefix) {
    if (full.find(prefix) == 0) {
        return full.substr(prefix.size());  // Trả về phần còn lại sau prefix
    }
    return full;  // Nếu không có prefix, trả nguyên
}

boolean isPrefix(const string& full, const string& prefix) {
    if (full.find(prefix) == 0) {
        return true;
    }
    return false;
}

string formatFakePathToUnixStyle(const string& fake_path) {
    string unix_path = fake_path;
    for (char& c : unix_path) {
        if (c == '\\') c = '/';
    }
    return unix_path;
}


string getNormalizedCurrentDirectory() {
    char tempPath[MAX_PATH];
    // Lấy đường dẫn hiện tại (user nhập sao cũng được)
    DWORD len = GetCurrentDirectoryA(MAX_PATH, tempPath);
    if (len == 0 || len > MAX_PATH) {
        return "";  // Error
    }

    char normalizedPath[MAX_PATH];
    DWORD result = GetLongPathNameA(tempPath, normalizedPath, MAX_PATH);
    if (result == 0 || result > MAX_PATH) {
        // Nếu GetLongPathName fail, trả về như GetCurrentDirectory
        return string(tempPath);
    }

    return string(normalizedPath);
}


vector<string> formatToRealPath(vector<string> args) {
    vector<string> format_args = args;
    for(string &s : format_args){
        for(char &c : s){
            if(c == '/'){
                c = '\\';
            }
        }
    }
    return format_args;
}
