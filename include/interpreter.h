#pragma once
#include <vector>
#include <string>
using namespace std;


extern vector<string> builtin_str_script;
extern int (*builtin_func_script[])(vector<string>);

int find_builtin(const string& cmd);
int shell_run_script(vector<string> args);