#include "parsing/ast/if_node.h"
#include "parsing/condition_evaluator.h"
#include <iostream>

using namespace std;
IfNode::IfNode(const std::string& cond, Node* thenBlk, Node* elseBlk)
    : condition(cond), thenBlock(thenBlk), elseBlock(elseBlk) {}

IfNode::~IfNode() {
    delete thenBlock;
    delete elseBlock;
}


int IfNode::execute() {
    bool result = evaluate_condition(condition);
    if (result && thenBlock) return thenBlock->execute();
    if (!result && elseBlock) return elseBlock->execute();
    return 0;
}