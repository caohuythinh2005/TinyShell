#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include "utils.h"

using namespace std;

string trim(const string& s);
bool is_comment_or_empty(const string& line);
bool read_script_file(const string& filepath, vector<string>& script_lines);