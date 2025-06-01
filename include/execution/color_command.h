#ifndef COLOR_COMMAND_H
#define COLOR_COMMAND_H

#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include <cctype>

using namespace std;
extern WORD colorShell;
extern WORD colorCommand;
void saveConfig(const std::string& filename);
int shell_change_color(vector<string> args);

// Hàm đổi màu chữ sử dụng mã màu (Windows API)
void setTextColor(WORD color);

#endif // COLOR_COMMAND_H
