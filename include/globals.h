#ifndef GLOBALS_H
#define GLOBALS_H
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <vector>
#include <csignal>
using namespace std;


// global var
extern int status; /*flag to determine when to exit program*/
extern STARTUPINFO si;
extern PROCESS_INFORMATION pi;
extern string ROOT_DIRECTORY;  // Định nghĩa thư mục gốc
extern string origin_real_path;
extern string fixed_real_path;
extern string current_real_path;
extern string current_fake_path;
extern vector<string> builtin_str;
extern int (*builtin_func[]) (vector<string>);
extern HANDLE hJob;
extern JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli;
extern HANDLE fore;
#endif