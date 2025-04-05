#include "utils.h"
#include "constant.h"


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


