#pragma once
#include <string>
#include "parsing/ast/node.h"  // lớp cơ sở Node
using std::string;

class ForNode : public Node {
public:
    ForNode(const string& initExpr, const string& condExpr, const string& incExpr, Node* body);
    ~ForNode();

    int execute() override;

private:
    string init_expr;
    string condition_expr;
    string increment_expr;
    Node* body;
};