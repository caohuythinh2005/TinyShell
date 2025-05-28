
#include "parsing/ast/block_node.h"

void BlockNode::addStatement(Node* stmt) {
    statements.push_back(stmt);
}

int BlockNode::execute() {
    for (Node* stmt : statements) {
        if (stmt->execute() != 0) return -1;
    }
    return 0;
}

BlockNode::~BlockNode() {
    for (Node* stmt : statements)
        delete stmt;
}