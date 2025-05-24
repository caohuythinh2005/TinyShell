#pragma once
#define _HAS_STD_BYTE 0
#include <string>
#include <vector>

using namespace std;
class Node {
public:
    virtual int execute() = 0;
    virtual ~Node() {}

    // static Node* parse(const vector<string>& lines, size_t& index);
};