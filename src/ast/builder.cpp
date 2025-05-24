#include "ast/builder.h"
#include "ast/command_node.h"
#include "ast/block_node.h"
#include "system_commands.h"
#include "utils.h"
#include "ast/if_node.h"
#include "ast/gb.h"
#include <iostream>

using namespace std;


Node* parse_block(const vector<string>& lines, size_t& index) {
    BlockNode* block = new BlockNode();

    while (index < lines.size()) {
        string line = trim(lines[index]);

        if (line.empty() || line[0] == '#') {
            ++index;
            continue;
        }

        // Nếu gặp dấu '}', block kết thúc ngay
        if (line == "}") {
            ++index; // tăng qua dấu '}'
            break;
        }

        vector<string> tokens = parse_command(line);
        if (tokens.empty()) {
            ++index;
            continue;
        }

        const string& cmd = tokens[0];

        if (cmd == "if") {
            string condition = trim(line.substr(2));
            ++index;

            if (index < lines.size() && trim(lines[index]) == "{") ++index;

            Node* thenNode = parse_block(lines, index);

            // Không tăng index ở đây vì parse_block đã tăng qua '}'

            Node* elseNode = nullptr;
            if (index < lines.size() && trim(lines[index]) == "else") {
                ++index;
                if (index < lines.size() && trim(lines[index]) == "{") {
                    ++index;
                    elseNode = parse_block(lines, index);
                    // parse_block tăng qua '}'
                } else {
                    elseNode = parse_block(lines, index);
                }
            }

            block->addStatement(new IfNode(condition, thenNode, elseNode));
        }
        else if (cmd == "while") {
            string condition = trim(line.substr(5));
            ++index;

            if (index < lines.size() && trim(lines[index]) == "{") ++index;

            Node* bodyNode = parse_block(lines, index);

            block->addStatement(new WhileNode(condition, bodyNode));
        }
        else {
            block->addStatement(new CommandNode(tokens));
            ++index;
        }
    }

    return block;
}


Node* build(const vector<string>& script_lines) {
    size_t index = 0;
    return parse_block(script_lines, index);
}