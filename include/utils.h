#ifndef _UTILS_H_
#define _UTILS_H_
#include "globals.h"
#include <filesystem>

string removePrefix(const string& full, const string& prefix);

boolean isPrefix(const string& full, const string& prefix);

#endif