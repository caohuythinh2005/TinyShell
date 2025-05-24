#include "ast/command_node.h"
#include "system_commands.h"
#include <iostream>

using namespace std;

CommandNode::CommandNode(const vector<string>& arguments) {
    if (!arguments.empty()) {
        command = arguments[0];
        args = arguments;
    }
}

int CommandNode::execute() {
    // cout << "[Command] Executing: " << command << endl;
    shell_working(args);
    return 0;
}