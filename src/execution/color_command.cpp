#include "execution/color_command.h"

void printColorTable() {
    std::cout << "\nColor Codes:\n";
    std::cout << "+-------+------------------+\n";
    std::cout << "| Code  | Color Name       |\n";
    std::cout << "+-------+------------------+\n";
    std::cout << "| 0     | Black            |\n";
    std::cout << "| 1     | Blue             |\n";
    std::cout << "| 2     | Green            |\n";
    std::cout << "| 3     | Aqua             |\n";
    std::cout << "| 4     | Red              |\n";
    std::cout << "| 5     | Purple           |\n";
    std::cout << "| 6     | Yellow           |\n";
    std::cout << "| 7     | White            |\n";
    std::cout << "| 8     | Gray             |\n";
    std::cout << "| 9     | Light Blue       |\n";
    std::cout << "| A     | Light Green      |\n";
    std::cout << "| B     | Light Aqua       |\n";
    std::cout << "| C     | Light Red        |\n";
    std::cout << "| D     | Light Purple     |\n";
    std::cout << "| E     | Light Yellow     |\n";
    std::cout << "| F     | Bright White     |\n";
    std::cout << "+-------+------------------+\n";
}

int shell_change_color(vector<string> args) {
    if (args.size() < 2 || args.size() > 3) {
        std::cout << "Usage: color [-s|-c|-h] [color-code]\n";
        std::cout << "  -h          Show this help message\n";
        std::cout << "  -s [code]   Set shell prompt color\n";
        std::cout << "  -c [code]   Set command text color\n";
        printColorTable();
        return 1;
    }

    if (args.size() == 2 && args[1] == "-h") {
        printColorTable();
        return 1;
    }

    if (args.size() != 3) {
        std::cout << "Usage: color [-s|-c|-h] [color-code]\n";
        std::cout << "  -h          Show this help message\n";
        std::cout << "  -s [code]   Set shell prompt color\n";
        std::cout << "  -c [code]   Set command text color\n";
        printColorTable();
        return 1;
    }

    string flag = args[1];
    string code = args[2];

    if (code.size() != 1 || !isxdigit(code[0])) {
        std::cout << "Invalid color code. Use a hex digit (0-F).\n";
        return 1;
    }

    int colorValue;
    if (isdigit(code[0])) {
        colorValue = code[0] - '0';
    } else {
        colorValue = toupper(code[0]) - 'A' + 10;
    }

    WORD newColor = (WORD)(colorValue | FOREGROUND_INTENSITY);

    if (flag == "-s") {
        colorShell = newColor;
        setTextColor(colorShell);
    }
    else if (flag == "-c") {
        colorCommand = newColor;
    }
    else {
        std::cout << "Invalid option. Use -s, -c, or -h.\n";
        printColorTable();
        return 1;
    }

    saveConfig("config\\.myshell_config");  // lưu thay đổi vào file config

    return 1;
}

// Thay đổi màu chữ hiện tại trên console
void setTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

