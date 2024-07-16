#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

    void hide_process(const char *process_name) {
        HANDLE hSnapshot;
        PROCESSENTRY32 pe32;
        hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hSnapshot == INVALID_HANDLE_VALUE) {
            printf("Error creating snapshot.\n");
            return;
        }

        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (!Process32First(hSnapshot, &pe32)) {
            CloseHandle(hSnapshot);
            printf("Error getting first process.\n");
            return;
        }

        do {
            if (strcmp(pe32.szExeFile, process_name) == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                if (hProcess) {
                    printf("Hiding process: %s (PID: %d)\n", pe32.szExeFile, pe32.th32ProcessID);
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                }
            }
        } while (Process32Next(hSnapshot, &pe32));

        CloseHandle(hSnapshot);
    }

    int main() {
        hide_process("notepad.exe");
        return 0;
    }