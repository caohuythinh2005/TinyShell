#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "globals.h"

/*
Yêu cầu : 
- Bắt toàn bộ tiến trình liên quan đến shell (tiến trình cha và con),
- Chỉ lưu những tiến trình do shell tạo ra hoặc chính shell
- Lưu vào vector chứa ProcessInfor,
*/


int init_process();

vector<ProcessInfor> getShellProcesses();
void printProcesses(const vector<ProcessInfor>& processes);

// int shell_killProcessByName(vector<string> args);
// int shell_suspendByName(vector<string> args);
// int shell_resumeByName(vector<string> args);
vector<ProcessInfor> getShellProcesses();
vector<ProcessInfor> getShellProcessesWithStatus();
bool isProcessSuspended(DWORD pid);

int shell_killProcessById(vector<string> args);
int shell_suspendById(vector<string> args);
int shell_resumeById(vector<string> args);


int shell_runExe(vector<string> args);
int shell_runScript(vector<string> args);

int shell_list(vector<string> args);



#endif
