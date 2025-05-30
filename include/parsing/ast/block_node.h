#pragma once
#include "parsing/ast/node.h" 
#include <vector>

using namespace std;

class BlockNode : public Node {
    std::vector<Node*> statements;

public:
    void addStatement(Node* stmt);
    int execute() override;
    ~BlockNode();
};