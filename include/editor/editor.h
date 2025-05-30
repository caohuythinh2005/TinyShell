#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

class Editor {
public:
    Editor(const std::string& filename);
    void run();

private:
    std::vector<std::string> lines = {""};
    int cx = 0, cy = 0;
    bool insertMode = false;
    std::string filename;

    std::string trim(const std::string& s);
    void loadFile();
    void saveFile();
    void draw(bool command_mode, const std::string& command);
    void handleInput();
};

int shell_editor(std::vector<std::string> args);

#endif
