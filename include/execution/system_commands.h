#ifndef _SYSTEM_COMMANDS_H_
#define _SYSTEM_COMMANDS_H_

#include "utilities/globals.h"

void init_system_commands();
string read_command_line();
vector<string> parse_command(string line);
void shell_working(vector<string> args);
int shell_help(vector<string> args);
// int shell_print_processes_info(vector<string> args);
int shell_cls(vector<string> args);
int shell_exit(vector<string> args);
int shell_time(vector<string> args);
int shell_date(vector<string> args);
vector<string> get_autocomplete_candidates(const string& prefix);


#endif