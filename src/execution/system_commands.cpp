#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <sys/stat.h>
#include <sstream>
#include "utilities/globals.h"
#include "filesystem/directory_manager.h"
#include "utilities/constant.h"
#include "execution/system_commands.h"
#include "filesystem/path_manager.h"
#include "execution/process.h"
#include "utilities/cal.h"
#include <conio.h>
#include "utilities/utils.h"
#include "execution/color_command.h"

vector<string> history;
int history_pos = -1;

void init_system_commands()
{
    status = 1;
}

int shell_help(std::vector<std::string> args)
{
    if (args.size() == 1)
    {
        cout << "\n====================================================================================\n";
        // Navigation
        cout << "\nDirectory Navigation:\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";
        cout << "| cd [path]                 | Change current directory to [path]                 |\n";
        cout << "| cd ..                     | Move to the parent directory                       |\n";
        cout << "| pwd                       | Show the current working directory                 |\n";
        cout << "| dir                       | List files and folders in the current directory    |\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";

        // File operations
        cout << "\nFile and Folder Operations:\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";
        cout << "| mkdir [name]              | Create a new directory                             |\n";
        cout << "| del [name]                | Delete a file or folder (recursive if folder)      |\n";
        cout << "| touch [-f] [name]         | Create empty file; overwrite if -f                 |\n";
        cout << "| cat [name]                | Display contents of a file                         |\n";
        cout << "| write [-f|-a] [name]      | Write to file; -f overwrite, -a append             |\n";
        cout << "| rename [old] [new]        | Rename a file or folder                            |\n";
        cout << "| move [src] [dest]         | Move file or folder                                |\n";
        cout << "| copy [src] [dest]         | Copy file or folder                                |\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";

        // Process management
        cout << "\nProcess Management:\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";
        cout << "| exec [path] [-b|-c]       | Execute .exe or .bat file. Options -b, -c (.exe)   |\n";
        cout << "| kill_id [pid]             | Kill managed process by PID                        |\n";
        cout << "| pause_id [pid]            | Suspend managed process by PID                     |\n";
        cout << "| resume_id [pid]           | Resume suspended process by PID                    |\n";
        cout << "| list                      | List processes managed by TinyShell                |\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";

        // Environment variables
        cout << "\nEnvironment Variables:\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";
        cout << "| set                       | Display all environment variables                  |\n";
        cout << "| set VAR=value             | Set or update a variable                           |\n";
        cout << "| set VAR=                  | Delete a variable                                  |\n";
        cout << "| set /a expression         | Evaluate arithmetic expression (like VAR=1+2)      |\n";
        cout << "| setx VAR=value            | Persistently set variable                          |\n";
        cout << "| path                      | Show system PATH variable                          |\n";
        cout << "| addpath <path>            | Add new directory to PATH (session only)           |\n";
        cout << "| addpathx <path>           | Add new directory to PATH (permanent)              |\n";
        cout << "| delpath <path>            | Remove directory from PATH (session only)          |\n";
        cout << "| delpathx <path>           | Remove directory from PATH (permanent)             |\n";
        cout << "| where                     | Show full path of a .bat or .exe file              |\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";

        // Utility and system
        cout << "\nSystem Utilities:\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";
        cout << "| time                      | Show system time                                   |\n";
        cout << "| date                      | Show system date                                   |\n";
        cout << "| test [-f|-d] [path]       | Check if file (-f) or directory (-d) exists        |\n";
        cout << "| cal [num1] [op] [num2]    | Perform calculation (+ - * / %)                    |\n";
        cout << "| cls                       | Clear the console screen                           |\n";
        cout << "| exit                      | Exit the shell                                     |\n";
        cout << "| help                      | Show this help message                             |\n";
        cout << "| tab                       | Autocomplete command or filename                   |\n";
        cout << "| tree [optional path]      | Display directory tree                             |\n";
        cout << "| color [-s|-c|-h] [code]   | Change shell (-s) or command (-c) color; -h : help |\n";
        cout << "| name [new_name]           | Change shell prompt name                           |\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";

        // Editor
        cout << "\nBuilt-in Text Editor:\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";
        cout << "| editor -i <file>          | Open file in interactive editor                    |\n";
        cout << "| editor -h                 | Show key bindings for editor                       |\n";
        cout << "+---------------------------+-----------------------------------------------------+\n";

        // Control Structures
        cout << "\nControl Structures:\n";
        cout << "+--------------------------------------------+-------------------------------------------------------------+\n";
        cout << "| if <cond>                                  | Execute block if condition is true                          |\n";
        cout << "| if <cond> else                             | Execute second block if condition is false                  |\n";
        cout << "| while <cond>                               | Repeat block while condition is true                        |\n";
        cout << "| for (<var>=<start>;<cond>;<update>)        | Classic for-loop (like C-style)                             |\n";
        cout << "| eval <expression>                          | Evaluate logical/arithmetic expression                      |\n";
        cout << "+--------------------------------------------+-------------------------------------------------------------+\n";

        cout << "\n====================================================================================\n";
    }
    else
    {
        cout << "Bad command...\n";
        return BAD_COMMAND;
    }

    return 0;
}

void shell_working(vector<string> args)
{
    int shell_num_builtins = builtin_str.size();
    int check = 0;
    for (int i = 0; i < shell_num_builtins; i++)
    {
        if (args[0] == builtin_str[i])
        {
            check = 1;
            (*builtin_func[i])(args);
            return;
        }
    }
    if (check == 0)
    {
        printf("Bad command....\n");
    }
}

// Lấy danh sách lệnh bắt đầu với prefix
vector<string> get_autocomplete_candidates(const string &prefix)
{
    vector<string> candidates;
    for (const auto &cmd : builtin_str)
    {
        if (cmd.find(prefix) == 0)
        {
            candidates.push_back(cmd);
        }
    }
    return candidates;
}

vector<string> list_directory_with_prefix(const string &directory, const string &prefix)
{
    vector<string> result;

    // Chuẩn hóa directory: nếu không có \ cuối thì thêm
    string norm_dir = directory;
    if (!norm_dir.empty() && norm_dir.back() != '\\' && norm_dir.back() != '/')
        norm_dir += "\\";

    string search_path = norm_dir + "*";

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(search_path.c_str(), &findData);

    // cout << "\nDanh sach cac ung vien:\n";

    if (hFind == INVALID_HANDLE_VALUE)
        return result;

    do
    {
        string name = findData.cFileName;

        // Bỏ "." và ".."
        if (name == "." || name == "..")
            continue;

        // Kiểm tra prefix (không phân biệt hoa thường)
        if (name.size() >= prefix.size())
        {
            bool match = true;
            for (size_t i = 0; i < prefix.size(); ++i)
            {
                if (tolower(name[i]) != tolower(prefix[i]))
                {
                    match = false;
                    break;
                }
            }
            if (match)
            {
                // cout << name << endl;
                result.push_back(name);
            }
        }
    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);
    return result;
}

// Hàm tách line thành tokens (theo dấu cách)
vector<string> split_tokens(const string &line)
{
    vector<string> tokens;
    string token;
    for (char c : line)
    {
        if (c == ' ')
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        else
        {
            token += c;
        }
    }
    if (!token.empty())
        tokens.push_back(token);
    return tokens;
}

// // Trả về real path tương ứng với fake path, hoặc empty string nếu không hợp lệ
// string resolve_fake_path_to_real(const string& input_fake_path) {
//     if (input_fake_path.empty()) return current_real_path;

//     if (input_fake_path[0] == '/' || input_fake_path[0] == '\\') {
//         // Đường dẫn tuyệt đối trong fake path: chỉ chấp nhận /root
//         if (input_fake_path.size() >= 5 &&
//             (input_fake_path.substr(0, 5) == "/root" || input_fake_path.substr(0, 5) == "\\root")) {
//             string full_real_path = origin_real_path + input_fake_path;
//             full_real_path = normalize_path(full_real_path);
//             if (folderExists(full_real_path) == EXIST_FILE_OR_DIRECTORY) {
//                 return full_real_path;
//             } else {
//                 return ""; // không tồn tại
//             }
//         } else {
//             return ""; // sai đường dẫn tuyệt đối
//         }
//     } else if (input_fake_path == "..") {
//         if (current_fake_path == "\\root" || current_fake_path == "/root") {
//             return current_real_path; // không lên trên root
//         }
//         // Lên thư mục cha
//         size_t pos = current_real_path.find_last_of("\\/");
//         string parent_real = (pos == string::npos) ? current_real_path : current_real_path.substr(0, pos);
//         return parent_real;
//     } else if (input_fake_path == ".") {
//         return current_real_path;
//     } else {
//         // Đường dẫn tương đối nối với current_fake_path
//         string combined_fake;
//         if (!current_fake_path.empty() &&
//             (current_fake_path.back() == '\\' || current_fake_path.back() == '/')) {
//             combined_fake = current_fake_path + input_fake_path;
//         } else {
//             combined_fake = current_fake_path + "\\" + input_fake_path;
//         }
//         combined_fake = normalize_path(combined_fake);

//         string real_dir = origin_real_path;
//         if (!origin_real_path.empty() &&
//             origin_real_path.back() != '\\' && origin_real_path.back() != '/')
//             real_dir += "\\";
//         real_dir += combined_fake;
//         real_dir = normalize_path(real_dir);

//         if (folderExists(real_dir) == EXIST_FILE_OR_DIRECTORY &&
//             isPrefix(real_dir, fixed_real_path)) {
//             return real_dir;
//         } else {
//             return "";
//         }
//     }
// }

string read_command_line()
{
    string line;
    int pos = 0;
    static vector<string> history;
    static int history_index = -1;

    while (true)
    {
        int ch = _getch();

        if (ch == 224)
        {
            int ch2 = _getch();
            if (ch2 == 72) // UP
            {
                if (!history.empty())
                {
                    if (history_index == -1)
                        history_index = (int)history.size() - 1;
                    else if (history_index > 0)
                        history_index--;

                    // Xóa dòng cũ
                    for (int i = 0; i < (int)line.size(); ++i)
                        cout << "\b \b";
                    line = history[history_index];

                    setTextColor(colorCommand); // *** Thiết lập màu trước khi in ***
                    cout << line;
                    pos = (int)line.size();
                }
            }
            else if (ch2 == 80) // DOWN
            {
                if (!history.empty() && history_index != -1)
                {
                    history_index++;
                    if (history_index >= (int)history.size())
                    {
                        history_index = -1;
                        for (int i = 0; i < (int)line.size(); ++i)
                            cout << "\b \b";
                        line.clear();
                        pos = 0;
                    }
                    else
                    {
                        for (int i = 0; i < (int)line.size(); ++i)
                            cout << "\b \b";
                        line = history[history_index];

                        setTextColor(colorCommand); // *** Thiết lập màu trước khi in ***
                        cout << line;
                        pos = (int)line.size();
                    }
                }
            }
            else if (ch2 == 75) // LEFT
            {
                if (pos > 0)
                {
                    cout << '\b';
                    pos--;
                }
            }
            else if (ch2 == 77) // RIGHT
            {
                if (pos < (int)line.size())
                {
                    setTextColor(colorCommand); // *** Thiết lập màu trước khi in ***
                    cout << line[pos];
                    pos++;
                }
            }
            continue;
        }

        if (ch == 13) // ENTER
        {
            cout << "\n";
            if (!line.empty())
                history.push_back(line);
            history_index = -1;
            break;
        }
        else if (ch == 8) // BACKSPACE
        {
            if (!line.empty() && pos > 0)
            {
                line.erase(pos - 1, 1);
                pos--;
                cout << "\b \b";
                setTextColor(colorCommand); // *** Thiết lập màu ***
                for (size_t i = pos; i < line.size(); ++i)
                    cout << line[i];
                cout << ' ';
                for (size_t i = pos; i <= line.size(); ++i)
                    cout << '\b';
            }
        }
        else if (ch == 9) // TAB
        {
            // ... (để nguyên phần xử lý tab, chỉ cần thêm setTextColor khi in ra)
            vector<string> tokens = split_tokens(line);
            string prefix;
            int token_start_pos = 0;

            if (!tokens.empty())
            {
                prefix = tokens.back();

                size_t found = line.rfind(prefix, pos);
                if (found != string::npos)
                    token_start_pos = (int)found;
                else
                    token_start_pos = (int)(line.size() - prefix.size());
            }

            vector<string> candidates;
            bool check = true;

            if (tokens.size() <= 1)
            {
                candidates = get_autocomplete_candidates(prefix);
            }
            else
            {
                size_t last_slash = prefix.find_last_of("\\/");
                string directory = current_real_path + "\\";
                string file_prefix = prefix;
                string dir_prefix = "";

                if (last_slash != string::npos)
                {
                    string relative_dir = prefix.substr(0, last_slash + 1);
                    file_prefix = prefix.substr(last_slash + 1);
                    string fake_dir;

                    if (!relative_dir.empty() && (relative_dir[0] == '/' || relative_dir[0] == '\\'))
                        fake_dir = relative_dir.substr(1);
                    else if (!current_fake_path.empty())
                        fake_dir = current_fake_path + "\\" + relative_dir;
                    else
                        fake_dir = relative_dir;

                    string real_dir = origin_real_path;
                    if (!origin_real_path.empty() &&
                        origin_real_path.back() != '\\' && origin_real_path.back() != '/')
                        real_dir += "\\";
                    real_dir += fake_dir;

                    if (SetCurrentDirectory(real_dir.c_str()) == FALSE)
                    {
                        candidates.clear();
                        continue;
                    }

                    real_dir = getNormalizedCurrentDirectory();

                    if (!isPrefix(real_dir, fixed_real_path))
                    {
                        if (isPrefix(fixed_real_path, real_dir))
                        {
                            directory = formatFakePathToUnixStyle(convertRealToFakePath(fixed_real_path));
                            candidates.push_back(directory);
                            check = false;
                        }
                        else
                        {
                            candidates.clear();
                            SetCurrentDirectory(origin_real_path.c_str());
                            continue;
                        }
                    }

                    if (check)
                        directory = real_dir;

                    dir_prefix = "";
                    SetCurrentDirectory(origin_real_path.c_str());
                }

                if (check)
                {
                    candidates = list_directory_with_prefix(directory, file_prefix);
                    if (candidates.empty())
                        continue;

                    for (auto &c : candidates)
                        c = dir_prefix + c;

                    for (auto &c : candidates)
                    {
                        string raw_path = convertRealToFakePath(directory) + '/' + c;
                        string formatted;
                        bool last_was_slash = false;
                        for (char ch : raw_path)
                        {
                            if (ch == '/' || ch == '\\')
                            {
                                if (!last_was_slash)
                                {
                                    formatted += '/';
                                    last_was_slash = true;
                                }
                            }
                            else
                            {
                                formatted += ch;
                                last_was_slash = false;
                            }
                        }
                        if (formatted.size() > 1 && formatted.back() == '/')
                            formatted.pop_back();
                        c = formatted;
                    }
                }
            }

            if (candidates.empty())
            {
                continue;
            }
            else if (candidates.size() == 1)
            {
                string completion = candidates[0];

                int erase_count = (int)(pos - token_start_pos);
                for (int i = 0; i < erase_count; ++i)
                {
                    cout << "\b \b";
                }

                line.erase(token_start_pos, erase_count);
                line.insert(token_start_pos, completion);

                setTextColor(colorCommand); // *** Thiết lập màu trước khi in ***
                cout << completion;

                pos = token_start_pos + (int)completion.size();
            }
            else
            {
                cout << "\n";
                setTextColor(colorCommand); // *** Thiết lập màu trước khi in ***
                for (const auto &c : candidates)
                    cout << c << "  ";
                cout << "\n> ";

                setTextColor(colorCommand); // *** Thiết lập màu trước khi in ***
                cout << line;

                for (int i = (int)line.size(); i > pos; --i)
                    cout << '\b';
            }
        }
        else if (ch >= 32 && ch <= 126) // printable characters
        {
            line.insert(pos, 1, (char)ch);
            ++pos;

            setTextColor(colorCommand); // *** Thiết lập màu trước khi in ***
            cout << line.substr(pos - 1);
            cout << ' ';
            for (size_t i = pos; i <= line.size(); ++i)
                cout << '\b';
        }
    }

    return line;
}

vector<string> parse_command(string line)
{
    vector<string> args;
    string arg;

    for (char ch : line)
    {
        if (isspace(ch))
        {
            if (!arg.empty())
            {
                args.push_back(arg);
                arg.clear();
            }
        }
        else
        {
            arg.push_back(ch);
        }
    }

    // Nếu sau khi duyệt hết mà vẫn còn một token chưa được thêm vào, thêm vào args
    if (!arg.empty())
    {
        args.push_back(arg);
    }

    return args;
}

int shell_exit(vector<string> args)
{
    if (args.size() > 1)
    {
        printf("Bad command ... \n");
        return BAD_COMMAND;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    exit(EXIT_SUCCESS);
    return 0;
}

int shell_cls(vector<string> args)
{
    if (args.size() > 1)
    {
        printf("\nBad command ....\n\n");
        return 0;
    }
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return ERROR_CLS;
    }

    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    if (!FillConsoleOutputCharacter(hConsole,        // Handle đến console screen buffer
                                    (TCHAR)' ',      // Ký tự muốn ghi
                                    dwConSize,       // Số lượng ký tự cần ghi
                                    coordScreen,     // Tọa độ muốn ghi
                                    &cCharsWritten)) // Số lượng ký tự thực tế đã ghi vào console
    {
        return ERROR_CLS;
    }

    // Lấy lại thông tin màn hình
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return ERROR_CLS;
    }

    // Đặt lại các thuộc tính cho buffer
    if (!FillConsoleOutputAttribute(hConsole,         // Handle đến console screen buffer
                                    csbi.wAttributes, // Thuộc tính ký tự sử dụng
                                    dwConSize,        // Số lượng ký tự thiết lập
                                    coordScreen,      // Tọa độ
                                    &cCharsWritten))  // Số lượng ký tự thực tế
    {
        return 0;
    }

    // Đặt lại vị trí
    SetConsoleCursorPosition(hConsole, coordScreen);
    return 0;
}

int shell_time(vector<string> args)
{
    if (args.size() != 1)
    {
        printf("Bad command....\n");
        return 0;
    }

    SYSTEMTIME lt = {0};
    GetLocalTime(&lt);

    // Tên thứ trong tuần (0 = Chủ Nhật)
    const wchar_t* weekdays[] = {
        L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday"
    };

    wprintf(L"\n=== Current Local Time ===\n");
    wprintf(L"Date: %04d-%02d-%02d (%s)\n", lt.wYear, lt.wMonth, lt.wDay, weekdays[lt.wDayOfWeek]);
    wprintf(L"Time: %02d:%02d:%02d\n\n", lt.wHour, lt.wMinute, lt.wSecond);

    return 0;
}

int shell_date(vector<string> args)
{
    if (args.size() != 1)
    {
        printf("Bad command....\n");
        return 0;
    }

    SYSTEMTIME st = {0};
    GetLocalTime(&st);

    // Tên thứ trong tuần (0 = Chủ Nhật)
    const wchar_t* weekdays[] = {
        L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday"
    };

    wprintf(L"\n=== Current Local Date ===\n");
    wprintf(L"Today is: %04d-%02d-%02d (%s)\n\n", st.wYear, st.wMonth, st.wDay, weekdays[st.wDayOfWeek]);

    return 0;
}