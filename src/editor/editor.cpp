#include "editor/editor.h"
#include <conio.h>  // Windows-only
#include <cstdlib>  // system("cls")
#include "directory_manager.h"
#include "system_commands.h"
Editor::Editor(const std::string& fname) : filename(fname) {
    loadFile();
}

std::string Editor::trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && isspace(*start)) start++;
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && isspace(*end));
    return std::string(start, end + 1);
}

void Editor::loadFile() {
    std::ifstream in(filename);
    if (!in) return;
    lines.clear();
    std::string line;
    while (std::getline(in, line)) lines.push_back(line);
    if (lines.empty()) lines.push_back("");
}

void Editor::saveFile() {
    std::ofstream out(filename);
    for (auto& l : lines) out << l << std::endl;
}

void Editor::draw(bool command_mode, const std::string& command) {
    system("cls");
    for (size_t i = 0; i < lines.size(); ++i) {
        if (i == cy) {
            std::string line = lines[i];
            if (cx > line.size()) cx = line.size();
            std::cout << line.substr(0, cx) << "|" << line.substr(cx) << std::endl;
        } else {
            std::cout << lines[i] << std::endl;
        }
    }
    if (command_mode) {
        std::cout << ":" << command;
    } else if (insertMode) {
        std::cout << "-- INSERT --";
    } else {
        std::cout << "-- NORMAL --";
    }
}

void Editor::handleInput() {
    std::string command;
    bool command_mode = false;

    while (true) {
        draw(command_mode, command);
        char ch = _getch();

        if (ch == 0 || ch == (char)224) {
            char ch2 = _getch();
            switch ((unsigned char)ch2) {
                case 72: if (cy > 0) --cy; if (cx > lines[cy].size()) cx = lines[cy].size(); break; // up
                case 80: if (cy + 1 < lines.size()) ++cy; if (cx > lines[cy].size()) cx = lines[cy].size(); break; // down
                case 75: if (cx > 0) --cx; break; // left
                case 77: if (cx < lines[cy].size()) ++cx; break; // right
                default: break;
            }
            continue;
        }

        if (command_mode) {
            if (ch == '\r' || ch == '\n') {
                command = trim(command);

                if (command == "q") break;
                else if (command == "w") {
                    saveFile();
                    command_mode = false;
                    command.clear();
                } else if (command == "wq") {
                    saveFile();
                    break;
                } else {
                    command_mode = false;
                    command.clear();
                }
            } else if (ch == 27) {
                command_mode = false;
                command.clear();
            } else {
                command.push_back(ch);
            }
            continue;
        }

        if (!insertMode) {
            if (ch == 'i') insertMode = true;
            else if (ch == ':') {
                command_mode = true;
                command.clear();
            }
            else if (ch == 'h' && cx > 0) --cx;
            else if (ch == 'l' && cx < lines[cy].size()) ++cx;
            else if (ch == 'j' && cy + 1 < lines.size()) ++cy;
            else if (ch == 'k' && cy > 0) --cy;
        } else {
            if (ch == 27 || ch == 9) insertMode = false;
            else if (ch == '\r') {
                std::string newLine = lines[cy].substr(cx);
                lines[cy] = lines[cy].substr(0, cx);
                lines.insert(lines.begin() + cy + 1, newLine);
                ++cy;
                cx = 0;
            }
            else if (ch == 8) {
                if (cx > 0) {
                    lines[cy].erase(cx - 1, 1);
                    --cx;
                } else if (cy > 0) {
                    cx = lines[cy - 1].size();
                    lines[cy - 1] += lines[cy];
                    lines.erase(lines.begin() + cy);
                    --cy;
                }
            } else {
                lines[cy].insert(cx, 1, ch);
                ++cx;
            }
        }
    }
}

void Editor::run() {
    handleInput();
}

int shell_editor(std::vector<std::string> args) {
    if (args.size() < 2) {
        std::cout << "Usage: editor <file>" << std::endl;
        return 1;
    }

    Editor editor(current_real_path + "\\" +  args[1]);
    editor.run();
    vector<string> dump;
    dump.push_back("cls");
    shell_cls(dump);
    return 0;
}
