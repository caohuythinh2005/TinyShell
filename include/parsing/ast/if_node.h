#pragma once
#include "parsing/ast/node.h" 
#include <string>
#include <vector>
#define _HAS_STD_BYTE 0
using namespace std;

class IfNode : public Node {
    std::string condition;
    Node* thenBlock;
    Node* elseBlock;

public:
    IfNode(const std::string& cond, Node* thenBlk, Node* elseBlk);
    ~IfNode();


    int execute() override;
};