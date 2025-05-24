#ifndef _UTILS_H_
#define _UTILS_H_
#include "globals.h"
#include <filesystem>


string trim(const string& s);
string removePrefix(const string& full, const string& prefix);

boolean isPrefix(const string& full, const string& prefix);

#endif