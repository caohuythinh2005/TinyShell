#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <sys/stat.h>
#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <psapi.h>
#include <sstream>
#include "path_manager.h"
#include "constant.h"
#include "utils.h"
#include "directory_manager.h"
#include <algorithm>
#include "globals.h"
#include "system_commands.h"
#include "cal.h"
#include <math.h>

double cal(const vector<string>& args) {
    if (args.size() != 3) {
        throw runtime_error("Invalid arithmetic expression. Use: <number> <operator> <number>\n");
    }

    double num1, num2;
    try {
        num1 = stod(args[0]);
        num2 = stod(args[2]);
    } catch (...) {
        throw runtime_error("Invalid.");
    }

    string op = args[1];
    if (op == "+") return num1 + num2;
    if (op == "-") return num1 - num2;
    if (op == "*") return num1 * num2;
    if (op == "/") {
        if (num2 == 0) throw runtime_error("Error: Divide by 0.");
        return num1 / num2;
    }
    if (op == "%") {
        if (num2 == 0) throw runtime_error("Error: Module 0.");
        return fmod(num1, num2);
    }
    if (op == "^"){
        if (num1 <= 0 && ((num1 - (int)num1) != 0) && (num2 - (int)num2) != 0) throw runtime_error("No negative real powers.");
        return pow(num1, num2);
    }
    throw runtime_error("Not support: " + op);
}

int shell_cal(vector<string> args){
    if(args.size() != 4){
        cout << "BAD COMMAND... \n";
        return BAD_COMMAND;
    }
    vector<string> exe;
    exe.push_back(args[1]), exe.push_back(args[2]), exe.push_back(args[3]);
    try {
        double result = cal(exe);
        printf("Value= %.2f\n", result);
        return 0;
    } catch (const runtime_error& e) {
        printf("Error: %s\n", e.what());
        return BAD_COMMAND;
    }
}