#pragma once

#include "ast/node.h"

#include <string>
#include <vector>

using namespace std;

Node* parse_block(const vector<string>& lines, size_t& index);
Node* build(const vector<string>& script_lines);