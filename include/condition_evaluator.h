#ifndef _CONDITION_EVALUATOR_H_
#define _CONDITION_EVALUATOR_H_

#include "globals.h"

enum ConditionType {
    EQUAL, NOT_EQUAL, LSS, LEQ, GTR, GEQ
};

int evaluate_condition(const string &left, const ConditionType &op, const string &right);
bool parse_condition(const string &condition_str, string &left, ConditionType &op, string &right);

#endif
