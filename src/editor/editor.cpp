#include "editor/editor.h"
#include <conio.h>  // Windows-only
#include <cstdlib>  // system("cls")
#include "filesystem/directory_manager.h"
#include "execution/system_commands.h"
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
        std::cout << "Usage:\n";
        std::cout << "  editor -i <file>   Edit a file\n";
        std::cout << "  editor -h          Show help\n";
        return 1;
    }

    if (args[1] == "-h") {
        std::cout << "\n=== KEY COMBINATION TABLE ===\n\n";
        std::cout << "| Key/Key Group         | Mode         | Function                                     |\n";
        std::cout << "|-----------------------|--------------|----------------------------------------------|\n";
        std::cout << "| i                     | Normal Mode  | Switch to Insert Mode                        |\n";
        std::cout << "| Esc                   | Insert Mode  | Return to Normal Mode                        |\n";
        std::cout << "| Ctrl + i (Tab)        | Insert Mode  | Return to Normal Mode                        |\n";
        std::cout << "| :                     | Normal Mode  | Enter Command Mode                           |\n";
        std::cout << "| Enter                 | Command Mode | Execute command (:w, :q, :wq, etc.)          |\n";
        std::cout << "| Esc                   | Command Mode | Cancel command input, return to Normal Mode  |\n";
        std::cout << "| Left Arrow            | Normal Mode  | Move cursor to the left                      |\n";
        std::cout << "| Down Arrow            | Normal Mode  | Move cursor down                             |\n";
        std::cout << "| Up Arrow              | Normal Mode  | Move cursor up                               |\n";
        std::cout << "| Right Arrow           | Normal Mode  | Move cursor to the right                     |\n";
        std::cout << "| Backspace             | Insert Mode  | Delete character before the cursor           |\n";
        std::cout << "| Enter                 | Insert Mode  | Insert a new line                            |\n";

        std::cout << "\n=== COMMANDS IN COMMAND MODE ===\n\n";
        std::cout << "| Command | Function                       |\n";
        std::cout << "|---------|--------------------------------|\n";
        std::cout << "| w       | Save current file              |\n";
        std::cout << "| q       | Quit the editor                |\n";
        std::cout << "| wq      | Save the file and quit         |\n";

        std::cout << std::endl;
    } else if (args[1] == "-i") {
        if (args.size() < 3) {
            std::cout << "Error: No file specified to edit.\n";
            std::cout << "Usage: editor -i <file>\n";
            return 1;
        }

        Editor editor(current_real_path + "\\" + args[2]);
        editor.run();

        std::vector<std::string> dump;
        dump.push_back("cls");
        shell_cls(dump);

        return 0;
    } else {
        std::cout << "Unknown option: " << args[1] << "\n";
        std::cout << "Use 'editor -h' for help.\n";
        return 1;
    }
}
