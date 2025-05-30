#ifndef _CONDITION_EVALUATOR_H_
#define _CONDITION_EVALUATOR_H_

#include "utilities/globals.h"

enum ConditionType {
    EQUAL, NOT_EQUAL, LSS, LEQ, GTR, GEQ
};


bool evaluate_condition(const string& expr);
int shell_eval(vector<string> args);
#endif
