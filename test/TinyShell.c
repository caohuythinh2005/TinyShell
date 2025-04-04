#include <Windows.h>
#include <stdio.h>

//int main() {
//    STARTUPINFO si;
//    PROCESS_INFORMATION pi;
//    ZeroMemory(&si, sizeof(si));
//    si.cb = sizeof(si);
//    CreateProcess("Child.exe", NULL, NULL, NULL, FALSE,
//        CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
//    WaitForSingleObject(pi.hProcess, INFINITE);
//    TerminateProcess(pi.hProcess, 0);
//    CloseHandle(pi.hProcess);
//    CloseHandle(pi.hThread);
//    return 0;
//}