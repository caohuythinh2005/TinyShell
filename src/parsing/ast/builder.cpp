#include "parsing/ast/builder.h"
#include "parsing/ast/command_node.h"
#include "parsing/ast/block_node.h"
#include "execution/system_commands.h"
#include "utilities/utils.h"
#include "parsing/ast/if_node.h"
#include "parsing/ast/gb.h"
#include <iostream>
#include "parsing/script_io.h"
#include "parsing/ast/for_node.h"
#include "filesystem/directory_manager.h"

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
        tokens = formatToRealPath(tokens);
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
        else if (cmd == "for") {
            string for_expr = trim(line.substr(3)); // lấy phần sau "for"

            // Bỏ ngoặc nếu có
            if (for_expr.front() == '(' && for_expr.back() == ')') {
                for_expr = for_expr.substr(1, for_expr.size() - 2);
            }

            // Tách 3 phần: init; condition; increment
            size_t semi1 = for_expr.find(';');
            size_t semi2 = for_expr.find(';', semi1 + 1);

            if (semi1 == string::npos || semi2 == string::npos) {
                cout << "Invalid for syntax. Expected: for (init; condition; increment)" << endl;
                ++index;
                continue;
            }

            string init = trim(for_expr.substr(0, semi1));
            string condition = trim(for_expr.substr(semi1 + 1, semi2 - semi1 - 1));
            string increment = trim(for_expr.substr(semi2 + 1));

            ++index; // qua dòng for

            // Nếu có dấu { mở block thì qua nó
            if (index < lines.size() && trim(lines[index]) == "{") ++index;

            Node* bodyNode = parse_block(lines, index);
            
            // cout << init << endl;
            // cout << condition << endl;
            // cout << increment << endl;
            // return nullptr;

            block->addStatement(new ForNode(init, condition, increment, bodyNode));
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

int shell_exec(vector<string> args) {
    for (string x: args) {
        cout << x << " ";
    }
    cout << endl;
    if (args.size() < 2) {
        cout << "Usage: exec <filename>" << endl;
        return -1;
    }
    vector<string> script_lines;
    if (!read_script_file(args[1], script_lines)) {
        return 1;
    }
    Node* root = build(script_lines);
    if (root) {
        root->execute();
        delete root;
    }
    return 0;
}
