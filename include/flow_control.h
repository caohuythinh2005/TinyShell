#ifndef _FLOW_CONTROL_H_
#define _FLOW_CONTROL_H_

#include "globals.h"
#include <map>
#include <string>

using namespace std;

// if ... else ...
int shell_if(vector<string> args);

// goto :label
int shell_goto(vector<string> args);

// build label->line mapping   --> kieu goto (set label)
void set_label_map(const vector<string> &script_lines); 

// for %%i in (...) do ...
int shell_for(vector<string> args);

// while ... do (mở rộng)
int shell_while(vector<string> args);

// do ... while (mở rộng)
int shell_do_while(vector<string> args);

// exit script
int shell_exit(vector<string> args);

#endif
