#ifndef _DIRECTORY_MANAGER_H_
#define _DIRECTORY_MANAGER_H_
#include "globals.h"

int init_directory();

// for shell
int shell_pwd(vector<string> args);
int shell_dir(vector<string> args);
int shell_cd(vector<string> args);
int shell_mkdir(vector<string> args);
int shell_del(vector<string> args);
string convertFakeToRealPath(const string &currentFakePath);
#endif