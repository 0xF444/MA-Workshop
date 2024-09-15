#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

void create_process() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));


    if(!CreateProcess(TEXT("C:\\Windows\\System32\\notepad.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Failed to create process. Error: %d\n", GetLastError());
        return;
    }

    printf("Created process: %d\n", pi.dwProcessId);


    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

DWORD find_process_by_name(const char* process_name) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (strcmp(pe.szExeFile, process_name) == 0) {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0;
}

void inject_code(DWORD process_id) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    if (hProcess == NULL) {
        printf("Failed to open process. Error: %d\n", GetLastError());
        return;
    }


    void* alloc_mem = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);


    unsigned char shellcode[] = "\x90\x90\x90\x90\xEB\xFE";  // NOPs and infinite loop


    WriteProcessMemory(hProcess, alloc_mem, shellcode, sizeof(shellcode), NULL);


    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)alloc_mem, NULL, 0, NULL);

    if (hThread == NULL) {
        printf("Failed to create remote thread. Error: %d\n", GetLastError());
        return;
    }

    printf("Injected code into process %d\n", process_id);

    CloseHandle(hProcess);
    CloseHandle(hThread);
}

int main() {

    create_process();
    DWORD process_id = find_process_by_name("notepad.exe");
    if (process_id == 0) {
        printf("Process not found\n");
        return 1;
    }

    inject_code(process_id);

    return 0;
}
