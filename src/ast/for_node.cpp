#include "ast/for_node.h"
#include "utils.h"         // chứa các hàm evaluate_condition, evaluate_assignment,...
#include <iostream>
#include "condition_evaluator.h"
#include "variable_manager.h"
using std::cout;
using std::endl;

ForNode::ForNode(const string& initExpr, const string& condExpr, const string& incExpr, Node* body)
    : init_expr(initExpr), condition_expr(condExpr), increment_expr(incExpr), body(body)
{}

ForNode::~ForNode() {
    delete body;
}

int ForNode::execute() {
    // Khởi tạo biến lặp
    evaluate_assignment(init_expr);

    // Vòng lặp điều kiện
    while (evaluate_condition(condition_expr)) {
        if (body) {
            body->execute();
        }
        // Cập nhật biến lặp
        evaluate_assignment(increment_expr);
    }
    return 0;
}