#pragma once
#include "node.h"
#include <string>
#include <vector>
#define _HAS_STD_BYTE 0
using namespace std;

class WhileNode : public Node {
    std::string condition;
    Node* body;

public:
    WhileNode(const std::string& cond, Node* bodyNode);
    ~WhileNode();

    int execute() override;
};