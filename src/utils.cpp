#include "utils.h"

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