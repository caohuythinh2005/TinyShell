#ifndef _UTILS_H_
#define _UTILS_H_
#include "globals.h"

extern string removePrefix(const string& full, const string& prefix);

extern boolean isPrefix(const string& full, const string& prefix);

extern string formatFakePathToUnixStyle(const string& fake_path);

extern string getNormalizedCurrentDirectory();

#endif