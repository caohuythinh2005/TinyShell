#include "parsing/ast/while_note.h"
#include "parsing/condition_evaluator.h"
#include <iostream>

using namespace std;

WhileNode::WhileNode(const std::string& cond, Node* bodyNode)
    : condition(cond), body(bodyNode) {}

WhileNode::~WhileNode() {
    delete body;
}

int WhileNode::execute() {
    while (evaluate_condition(condition)) {
        if (body->execute() != 0) return -1;
    }
    return 0;
}