#pragma once
#include <globals.h>

int shell_path(vector<string> args);
int shell_addpath(vector<string> args);
int shell_addpathx(vector<string> ags);
int shell_setP(vector<string> args);
int shell_runExe(vector<string> args);
int shell_delpath(vector<string> args);
int shell_delpathx(vector<string> args);
int shell_where(vector<string> args);
void initPath();
extern vector<string> envPaths;