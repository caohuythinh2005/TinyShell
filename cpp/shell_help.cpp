#include <iostream>

using namespace std;

int main() {
    cout << "\nAVAILABLE COMMANDS:\n";
    cout << "-------------------------------------------------------------\n";
    cout << "cls                          : Clear the console screen\n";
    cout << "exit                         : Exit the shell\n";
    cout << "pwd                          : Show the current working directory\n";
    cout << "dir                          : List all files and folders in the current directory\n";
    cout << "cd [path]                    : Change current directory to [path]\n";
    cout << "cd ..                        : Move to the parent directory\n";
    cout << "mkdir [name]                 : Create a new directory with given name\n";
    cout << "del [name]                   : Delete a file or folder (recursively if it's a folder)\n";
    cout << "help                         : Show this help message\n";
    cout << "test [-f|-d] [path]          : Check if the file or directory exists\n";
    cout << "exec [path]                  : Execute a bat file \n";
    cout << "time                         : Display current system time\n";
    cout << "date                         : Display current system date\n";
    cout << "runExe [path]                : Run executable at given path in foreground (press Ctrl+C to terminate)\n";
    cout << "runExe [path] -b             : Run executable in background (non-blocking)\n";
    cout << "kill_id [pid]                : Kill a process with the given PID (must be managed)\n";
    cout << "pause_id [pid]               : Suspend a managed process by PID\n";
    cout << "resume_id [pid]              : Resume a suspended managed process by PID\n";
    cout << "list                         : List all processes managed by TinyShell\n";
    cout << "touch [-f] [name]            : Create an empty file, overwrite if exists with -f\n";
    cout << "cat [name]                   : Display contents of a file\n";
    cout << "write [-f|-a] [name]         : Write to a file; -f to overwrite, -a to append\n";
    cout << "                               (default: overwrite if file exists, else create new)\n";
    cout << "rename [old_name] [new_name] : Rename file or folder\n";
    cout << "-------------------------------------------------------------\n";
}