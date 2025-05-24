#pragma once
#include "node.h"
#include <string>
#include <vector>

using namespace std;

class CommandNode : public Node {
    string command;
    vector<string> args;

public:
    CommandNode(const vector<string>& arguments);
    int execute() override;
};
