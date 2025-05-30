#ifndef _UTILS_H_
#define _UTILS_H_
#include "utilities/globals.h"
#include <filesystem>


string trim(const string& s);
string removePrefix(const string& full, const string& prefix);
string formatFakePathToUnixStyle(const string& fake_path);
boolean isPrefix(const string& full, const string& prefix);
vector<string> formatToRealPath(vector<string> fake_path);

#endif