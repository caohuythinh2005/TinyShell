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
int shell_test(vector<string> args);
string formatFakePathToUnixStyle(const string& fake_path);
string getNormalizedCurrentDirectory();
int fileExists(const string &path);
int folderExists(const string &path);
string convertFakeToRealPath(const string &currentFakePath);

#endif