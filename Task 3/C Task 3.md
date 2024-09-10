#resources/workshops/MAWorkshop/tasks 
# Outline
- [x] [Analysis Tasks](#Analysis%20Tasks)
	- [x] [Function Pointer Obfuscation.c](#Function%20Pointer%20Obfuscation.c)
	- [x] [Keylogger.c](#Keylogger.c)
	- [x] [Network Sniffer.c](#Network%20Sniffer.c)
	- [x] [Obfuscation with Control Flow Flattening.c](#Obfuscation%20with%20Control%20Flow%20Flattening.c)
	- [x] [Rootkit.c](#Rootkit.c)
	- [x] [Steganography-Based Data Exfiltration.c](#Steganography-Based%20Data%20Exfiltration.c)
- [x] [Reverse Tasks](#Reverse%20Tasks)
	- [x] [Data Hiding In Executable.c](#Data%20Hiding%20In%20Executable.c)
	- [x] [Hidden Functionality.c](#Hidden%20Functionality.c)
	- [x] [Encoded String Manipulation.c](#Encoded%20String%20Manipulation.c)
	- [x] [Self Modifying Code.c](#Self%20Modifying%20Code.c)
- [x] [Stack Research](#Stack%20Research)
- [x] [References](#References)
---
# Analysis Tasks
## Function Pointer Obfuscation.c
![Pasted image 20240715073502](../Assets/Pasted%20image%2020240715073502.png)

We first include our `stdio.h` library which has the `stdout/stdin` functions like `printf`.

Then we define 3 functions which prints three different lines. ^1c2824

![Pasted image 20240715075349](../Assets/Pasted%20image%2020240715075349.png)

In the `main()` function, a <mark style="background: #FFF3A3A6;"><mark style="background: #BBFABBA6;">function pointer</mark></mark> named `func_ptr` and a character variable named `choice` are declared, then the program prompts you to enter a number which will be assigned to the `choice` variable through the `scanf` function.

The program then assigns the address of [one of the three functions](#^1c2824) based on user input.

If the user inputs something other than `1, 2, 3`, then the program prints a message and returns 1 indicating <mark style="background: #FF5582A6;">failure</mark>.

Otherwise, the function selected is then called and the program returns 0, indicating <mark style="background: #BBFABBA6;">success</mark>.
## Keylogger.c

The program first begins by defining a *hook* global variable, then a **callback function** is declared with three arguments: `nCode`, `wParam` and `lParam`.

![Pasted image 20240715173855](../Assets/Pasted%20image%2020240715173855.png) ^e18af8

>[!note] Callback functions are functions that are used as parameters later on, it has a similar design to function pointers.
### KeyboardProc()
The function checks if the `nCode` parameter is equal to `HC_ACTION` which is a <mark style="background: #ABF7F7A6;">macro</mark> that indicates that the `wParam` and `lParam` arguments contain extra information about a keystroke message. ^e71c93

If the first condition has been met, the function then checks `wParam` if it contains the <mark style="background: #ABF7F7A6;">macro</mark> `WM_KEYDOWN` which indicate if the keystroke sent is a nonsystem key. ^d0720b
>[!note] A nonsystem key is a key that is pressed when the ALT key is not pressed.

If the [first](#^e71c93) and [second](#^d0720b) conditions have been met, the function then declares a pointer `p` of type `KBDLLHOOKSTRUCT` which will be initialized with `lParam` that is typecasted into `KBDLLHOOKSTRUCT` pointer.
>[!note] `lParam` here contains the extra information about the keystroke, such as the repeat count, scan code, etc..

Then a file descriptor variable `file` is defined and initialized by `fopen()` which will attempt to retrieve a file descriptor to `keylog.txt`, with **append** permissions, if the file doesn't exist then it will be created. ^7d5ed9

The function then checks if the [previous operation](#^7d5ed9) has succeeded, which if so, begins appending the character representation of the `vkCode` member pointed by the pointer `p`, then the file is properly closed.

The return value of this function is the return value of `CallNextHookEx()` which takes the `hHook` variable, and the arguments passed to the `KeyboardProc()` function.

>[!note] This return value ensures that the hook receives notification that `KeyboardProc()` has either succeeded or failed it's operation.

### KeyloggerThread()
A secondary function `KeyloggerThread()` is defined:

![Pasted image 20240715192552](../Assets/Pasted%20image%2020240715192552.png)

The function first obtains an instance of the current executable (exe or dll that loaded the program) under the variable `hInstance`. ^7b2cb2

Then the `hHook` [variable](#^e18af8) is set to a **low level keyboard hook** with [#KeyboardProc()](#KeyboardProc()) as a hook procedure in the `hInstance` [instance](#^7b2cb2).

Then the program defines a `msg` object in which the program attempts to retrieve every message in the thread's message queue, which is then *translated* and *dispatched* to the hook procedure.

>[!note] `TranslateMessage()` translates virtual-key messages to character messages and `DispatchMessage()` sends these messages to the hook procedure to write the content to the file.

After the program iterates over all the messages in the `msg` object, the `hHook` variable is *unhooked* by the `UnhookWindowsHookEx()` function.

>[!note] Unhooking here means that the hook procedure is removed from a hook chain.

### main()

![Pasted image 20240715195855](../Assets/Pasted%20image%2020240715195855.png)
The main function simply attempts to create a thread that executes the [#KeyloggerThread()](#KeyloggerThread()) function, which waits for a signaled state forever then closes.

### Conclusion
The program creates a hook procedure named `KeyboardProc()` which is triggered (Appends non-system keys captured to a file) when the `hHook` is set to the executable process and the message queue is sent to `KeyboardProc()`.
Then a thread executing `KeyloggerThread()` executes indefinitely until the thread is signaled, then the thread is closed.

## Network Sniffer.c

The program uses `stdio.h` and `pcap.h` libraries for input/output operations and to manage [pcap files](https://en.wikipedia.org/wiki/Pcap).

The program has two functions: `packet_handler()` and `main()`:
### packet_handler()

![Pasted image 20240716074849](../Assets/Pasted%20image%2020240716074849.png)
This is a simple function that takes the following arguments:

1. A pointer to an unsigned character named `param`
2. A pointer to a constant structure of `pcap_pkthdr` named `header`
3. A pointer to an unsigned character named `pkt_data`

All what it does is that it prints out the size of the packet header as pointed by the header pointer.

This function will serve as a **callback** function as this will be executed for every captured packet by 

### main()

![Pasted image 20240716084811](../Assets/Pasted%20image%2020240716084811.png)

Two pointers that point to a `pcap_if_t` structure are defined; named `alldevs` and `d` respectively.
>[!note] `pcap_if_t` structures have an item from interface lists.

One pointer that point to a `pcap_t` structure is defined; named `adhandle`
>[!note] `pcap_t` structures are descriptors of an open capture instance, it's like a "pointer" to the current live capture session.

Lastly, a buffer of size `PCAP_ERRBUF_SIZE` is declared, this will contain any error notifications, if any.

The program begins to check all interfaces in the `alldevs` pointer using `pcap_findalldevs()` with `errbuf` serving as the error buffer.

If the function above fails, then the program prints out the error code and exits, returning 1.

![Pasted image 20240716085401](../Assets/Pasted%20image%2020240716085401.png)
The program then iterates over each device found by `pcap_findalldevs()` and prints the description if found.

Then, the program attempts to open a handle to a packet capture descriptor (essentially opening a live capture session) using `pcap_open_live()`, if this fails, the programs prints the name of the faulty interface and exits prematurely.

The program then captures the packets from the device pointed by the descriptor `adhandle` and uses [packet_handler()](#packet_handler()) as a callback function (It is triggered for each packet captured)

Lastly, frees all interfaces from the `alldevs` interface list and exits, returning 0.

## Obfuscation with Control Flow Flattening.c

The program has 2 functions, `main()` which calls `obfuscated_code()`

### obfuscated_code()

![Pasted image 20240716183133](../Assets/Pasted%20image%2020240716183133.png)

This function implores a technique known as [Control Flow Flattening (CFF)](https://reverseengineering.stackexchange.com/questions/2221/what-is-a-control-flow-flattening-obfuscation-technique), which basically makes control flow a lot harder to read.

CFF has a few terminologies to help deobfuscate:
1. Dispatcher Block: This is the block that determines which block of the original code to be executed. ^ab71f2
2. State Variable: This is the variable that separates each block of the original code.

All what CFF does is that the original code is split up to different blocks, and modifies the state variable `state` for which the [Dispatcher Block](#^ab71f2) decides which case to be executed.

In our example here, the program here runs normally even with CFF, because each case changes the `state` variable to the next case statement, which means that we can remove the Dispatcher Block and the state variable and execute normally.

```c
void obfuscated_code(){
int x = 0;
int y = 5;
x = y + 2;
y = x - 3;
x = x + y;
printf("x: %d, y: %d\n", x, y);
}
```

The final result of the code is `x = 11`, `y = 4`.

## Rootkit.c

The program contains two functions: `hide_process()` and `main()`.

### hide_process()

This function takes a string `process_name` as an argument.

![Pasted image 20240717122941](../Assets/Pasted%20image%2020240717122941.png)

This function defines 2 important variables: a snapshot handle named `hSnapshot` and a `PROCESSENTRY32` structure named `pe32`.

The function attempts to obtain a handle to a snapshot where it contains ALL processes in the system using `CreateToolhelp32Snapshot`, as per defined by the `TH32CS_SNAPPROCESS` macro.

Then the function does a fail check, if the `hSnapshot` doesn't contain a valid handle to a snapshot object, it prints a message then returns.

After obtaining a valid snapshot handle, the program initializes the first member of the `pe32` structure, because otherwise, `Process32First()` <mark style="background: #FF5582A6;">fails.</mark>

The function then fetches the first process from the snapshot into the `pe32` structure and does a fail check.

![Pasted image 20240717124323](../Assets/Pasted%20image%2020240717124323.png)

Then the function attempts to iterate over the snapshot object in order to obtain the process that has the name `process_name` and then attempts to obtain a handle to that process (With all permissions), which if successful, prints a message displaying the name and the PID then terminates that process and closes the handle to it.

After obtaining the correct function, the program closes the handle to the snapshot and the function exits.

### main()

![Pasted image 20240717125011](../Assets/Pasted%20image%2020240717125011.png)

The main function here calls [#hide_process()](#hide_process()) passing `"Notepad.exe"` as a parameter.

## Steganography-Based Data Exfiltration.c 

This program has 2 functions: `hide_data()` which contain all the work, and the `main()` function calling it.

### hide_data()

![Pasted image 20240717193730](../Assets/Pasted%20image%2020240717193730.png)

The function attempts to open two files `img` and `dat` respectively, while `img` is in binary mode, and they both are opened with the same permission: R/W.

The function then calculates the size of the image by putting the cursor to <mark style="background: #BBFABBA6;">EOF</mark> using the `fseek()` function and then grabs the position of the cursor using `ftell()` and putting the result into `img_size`.
>[!note] This is an effective way of calculating the size of files.

The function then puts the position of the cursor back to the beginning of the file.

![Pasted image 20240717195355](../Assets/Pasted%20image%2020240717195355.png)

Then, the function allocates memory of size `img_size` from the heap using `malloc()` and assigns `buffer` as a pointer to that memory allocated.

The function then reads the image file stream and puts the contents read into the `buffer` memory block.

The function puts the cursor back to the start because `fread()` moves the cursor with each byte read.

The function then calls `fwrite()` to write back what's inside back into the image file. This is because the cursor will move to the end of the file, we basically append the image back from start of file, which will ensure that the cursor will be on EOF (End of File), which will make it easier to append our message to the end.

The function defines an unsigned char `ch` and then iterates over the `dat` file stream and writes it to the image file stream one byte/char at a time.

After the operation is complete, the buffer is freed and both files descriptors are closed.

### main()

![Pasted image 20240717205154](../Assets/Pasted%20image%2020240717205154.png)

The main function simply calls [#hide_data()](#hide_data()) with `image.bmp` and `secret.txt` as a parameter.

### Conclusion
This program writes all character bytes in `secret.txt` at the end of the `image.bmp` file.

![Pasted image 20240717205745](../Assets/Pasted%20image%2020240717205745.png)

---

# Reverse Tasks
## Data Hiding In Executable.c

### Solution
```c
#include <stdio.h>
#include <string.h>
char output_data[] = {0x1d,0x30,0x39,0x39,0x3a,0x55};
void print_output_data(){
    char *data = output_data;
    int cnt = 0;

for (char* item = output_data;cnt!=sizeof(output_data);item++,cnt++)
    *item = (char)((long)(*item)^0x55555555);
printf("%s",output_data);

return;
}
void main(){
    printf("Encoded Message: ");
    printf("%s",output_data);
    printf("\nDecoded Message: ");
    print_output_data();
    return;
}
```

### Output

![Pasted image 20240720033807](Assets/Pasted%20image%2020240720033807.png)

## Hidden Functionality.c
### Solution
```c
#include <stdio.h>
#include <string.h>
char key[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE};
char secret[] = {0x7F, 0x49, 0x2F, 0x12, 0xA3, 0xDF, 0x4B};
void hidden_feature(char *input) {
    int correct = 1;
    for (int i = 0; i < strlen(secret); i++) {
        if ((input[i] ^ key[i]) != secret[i]) {
            correct = 0;
            break;
        }
    }
    if (correct) {
        printf("Secret feature activated!\n");
    } else {
        printf("Access denied.\n");
    }
}

void main(){
    char input[sizeof(secret)];
    for (int i = 0; i < sizeof(secret); i++)
    {
        input[i] =(char) key[i] ^ (char) secret[i];
    }
    input[strcspn(input, "\n")] = 0;
    hidden_feature(input);
    return;
}
```

^9489e4

### Output

![Pasted image 20240720060445](Assets/Pasted%20image%2020240720060445.png)

## Encoded String Encapsulation.c
### Solution
```c
#include <stdio.h>
#include <string.h>
void decode(char *str){
    for (int i = 0; i < strlen(str); i++) {
        str[i] -= 3;
    }
}
int main() {
    char message[] = "Khoor#Zruog$";
    decode(message);
    printf("Decoded message: %s\n", message);
    return 0;
}
```

### Output

![Pasted image 20240720061804](Assets/Pasted%20image%2020240720061804.png)
## Self Modifying Code.c 
### Solution 
```c
#include <stdio.h>
#include <windows.h>

void remodify(char* obfuscated_code,int len){
    DWORD oldProtect;
    VirtualProtect(obfuscated_code, len, PAGE_EXECUTE_READWRITE, &oldProtect);
    for (int i = 0; i < len; i++) {
        obfuscated_code[i] ^= 0xFF;
    }
    VirtualProtect(obfuscated_code, len, oldProtect, &oldProtect);
}
int main() {
    char code[] = "\xaa\xb7\x76\x1a\x47\xfe\xff\xff\xff\xa2\x3c";
    remodify(code, sizeof(code) - 1);
    printf("Original Code: ");
    for (int i = 0; i < sizeof(code) - 1; i++) {
        printf("0x%02x ", (unsigned char)code[i]);
    }
    printf("\n");
    return 0;
}
```
### Output
![Pasted image 20240720161208](Assets/Pasted%20image%2020240720161208.png)

---
# Stack Research
### What is the stack?

The stack is a basic data structure that contains data in a "LIFO" (Last In First Out) principle, it exists in memory in a contiguous array of memory.

### Layout/Operation

The stack uses a general purpose register named <mark style="background: #BBFABBA6;">the stack pointer</mark> `RSP` which is used to point at the topmost element in the stack, it basically points at the most recent element.

Back in the Intel 8086 days, the stack was accessed by a segment register named [Stack Segment](https://en.wikipedia.org/wiki/X86_memory_segmentation) `SS`, it was used as a base address for which the stack pointer is used as an index to `SS`, this type of memory layout has been dropped in the x86-64 archtitecture.
>[!important] The stack segment was used because registers only had 16 bits of memory to address which is roughly 64 KB which is considered a bottleneck for 16 bit microprocessors, it works as a base and an offset is added to it in order to obtain the memory reference and allows for more memory to be addressed.

The stack has a weird layout (unlike normal memory address spaces), where if it grows, **it grows downwards** which decreases the memory address, and if it shrinks, it shrinks up, increasing the memory address. ^e7ec91

![Pasted image 20240721032855](Assets/Pasted%20image%2020240721032855.png)

There are two instructions related to stack operations: `push` and `pop`.
The `push` instruction takes one operand which is pushed onto the stack, and consequently [decreases the RSP pointer by the size of the data pushed, usually 8 bytes.](#^e7ec91) The `pop` instruction takes one operand which is the location of where you want to store your data and then removes the value that `RSP` is currently pointing at, and increasing the `RSP` pointer as a consequence.

`mov rbx, qword ptr ss:[rsp]` here tells the processor to take the "qword" (which is a quadword, same as 64-bit value) pointed by the Stack Reference `RSP` and copy it to `RBX`.

We may notice here that the `ss` register is mentioned, but this is to indicate that it is a stack reference, nothing more.

>[!note] Keep in mind:
>The `pop` instruction removes the element entirely from the stack to a different place, may it be memory location or a register, while a `mov` instruction from that same location simply copies that value to your designated location.

As a side note, programs can have <mark style="background: #ABF7F7A6;">many stacks</mark> (although most only have one) **but only one is available at a time**. 

### Calling

Both Calling and Returning use the stack in some way that we'll discuss briefly.

Remember when we talked about segment registers in the previous topic? Segment registers are mostly zeroed out in x64 architecture, except for the `GS` and `FS` registers are used by the operating system to store crucial information, for example Windows uses these registers to store the [TEB (Thread Environment Block)](https://en.wikipedia.org/wiki/Win32_Thread_Information_Block) in the `GS` register and the `FS` register is used for TLS (Thread Local Storage) which is private data related to each thread.

The `call` instructions has 4 type classifications:
- Near Call: Most commonly used and is used within the current code segment.
- Far Call: Can only be used with memory segmented models, allows you to call functions that are in a different code segment, thus modifying the `CS` register.
- Inter-Privilege-Level Far Call: Used to call functions from a different privilege level or "ring", such as from user mode to kernel mode. `sysenter` and `syscall` are a good example of this.
- Task Switch: Involves switching from one task to another, such calls are complex and are mostly dealt by the OS.

We're going to focus our scope on the Near Call type of `call` instructions are nowadays the [flat memory model](https://en.wikipedia.org/wiki/Flat_memory_model) is used and there's no need for segmentation.

The near call type has an opcode of `E8` while the far call type has the opcode of `FF`. 

There are two subtypes that are in near call types:
- Near Relative Calls: means that the call target address is "relative" to the address of the next instruction.
- Near Absolute Calls: Means that the call target address is "absolute" and specified indirectly is jumped to directly without any calculation.
I'll be showcasing the difference between each instruction:
```nasm
call some_function ; Near Relative Direct Call
call 0x7fff613a ; Near Absolute Direct Call (Rarely used)
call [rbx] ; Near Absolute Indirect Call
; Notice that all relative calls are direct and absolute calls are indirect.
```

Relative calls have their operand as the offset from the next instruction to start of our call target, since these operands exist in memory, they are viewed as little endian (Bytes are reversed). I'll be showcasing in example to this.

![Pasted image 20240721041542](Assets/Pasted%20image%2020240721041542.png)

We add the offset that leads us to the `MiProcessLoaderEntry` which is `FFA46496` (Notice the reversal) to the address of the next instruction which is `1406FB38E`.

>[!note] Important Note:
>The offset given is a negative because of the MSB (Most significant bit) of the offset is set, so we must sign extend this offset (Append more FFs to its left) in order to obtain the correct result, [check out 2's complement for further information](https://en.wikipedia.org/wiki/Two%27s_complement). 

### Call Stack Operations

When a near relative `call` instruction is executed, we first **push** the value of `RIP` (Instruction Pointer) to the stack, this is because it contains the address to the next instruction, this happens because when the program gives back control to its caller, it needs to know which instruction it needs to execute next.
>[!note] Keep in mind:
>The `RIP` value that is pushed is known as the return-instruction pointer.

### Returning

Basically means to "`ret`urn from current procedure", It gives transfers control from the callee function back to the **return-instruction pointer** (caller function) which is located at the most top of the stack (at index \[RSP]).

The `ret` instruction usually has the `C3` or `C2` opcodes, We'll be focusing on the `C3` near return instruction.

What it does is basically `pop` the return-instruction pointer back to the `RIP` register.

>[!important] Keep in mind:
>We can't manually modify `RIP` because that would make the computer vulnerable to execute arbitrary code, therefore processors use this safe method to modify the register.

### Calling Conventions
A Calling convention is basically a set of rules that functions follow in order to set up access to a subroutine, it specifies how arguments are passed to a function, where the return value of the function is placed, how to back up and restore old register values, how to create and manage stack frames, etc..

In the x86-64 architecture, ***fastcall*** is the most common calling convention.

There are many other calling conventions that were used back in the x86 days such as **stdcall**, **cdecl** and **fastcall**, but for the sake of this writeup we'll be focusing on **fastcall.**
>[!note] Side note:
>We can explicitly declare the calling conventions for functions:
>```c
>int __fastcall sub(int a, int b){
>return a-b;
>}
>```

Calling conventions are not really "standards" per say, but there are different methods that define how we can pass arguments to functions or managing stack frames such as the ABI (Application Binary Interface)

The ABI serves as the interface between an application and the OS or platform: It provides conventions and rules needed for datatypes, their sizes, alignment requirements, calling conventions, file formats, etc.. It allows the application to interact with the OS by it's bounded rules.

The code generation (part of the compiler process) must be aware and know the standards of the ABI.

### \_\_Fastcall Calling Convention

This convention uses four general purpose registers to pass **integer** arguments to the callee, namely: `rcx`, `rdx`, `r8` and `r9` respectively, if more arguments beyond these four exist, they're pushed onto the stack instead.
The order in which parameters are taken are from <mark style="background: #FF5582A6;">left to right</mark>, except when we push onto the stack (if the parameters exceed 4), the order is reversed to be from <mark style="background: #BBFABBA6;">right to left</mark>.

This conventions uses the `rax` register for **return values**.

> Arguments are **NEVER** split up in registers.

If we pass float-point arguments, we use the `XMM` registers which are special registers dedicated to floats and doubles.

When the caller is about to give control to the callee, it is it's responsibility to allocate space on the stack for the callee to save registers used to pass arguments because these registers that are used to pass arguments will be eventually be <mark style="background: #FF5582A6;">overwritten</mark> during the normal course of execution, this space freed is known as the "Shadow Space".

Shadow space can be as much as 32 bytes (Integers are 8 bytes each, 4 integers are passed onto the registers, 4\*8 = 32). ^e0e299

The stack must be **ALWAYS** aligned to a 16-byte boundary, what this means is that the starting address of the stack frame must be divisible by 16, this is to avoid stack misalignment which can cause the program to crash, and so we may allocate more space at the start with addition to [shadow space](#^e0e299) just to ensure this alignment.

![Pasted image 20240721075009](Assets/Pasted%20image%2020240721075009.png)

### Stack Frames

When functions begin their execution, they first push the old `rbp` register on the stack, this is to preserve the beginning of the caller's stack frame, the `rbp` register is used for stack frames and to address local variables or arguments in a function.
Then it moves whatever is on `rsp` to `rbp`, this effectively declares the new stack frame for the new function.

>[!note] Keep in mind 
>These two instructions are what's known as the prologue.
>```nasm
>push rbp
>mov rbp,rsp 
>```

![Pasted image 20240721080653](Assets/Pasted%20image%2020240721080653.png)
>[!important] Very Important Note:
>**Positive offsets** from RBP access **arguments passed** on the stack. **Negative offsets** from RBP access **local variables**.

### Passing Large Arguments

When passing large arguments such as objects, structures or strings, we just **pass them by reference**, we only need the object address in order to access it, that's how we access strings in C.

We can obtain the reference to a value by using the `lea` instruction which stands for "Load Effective Address".

---
# References
1. [Void Pointers](https://youtu.be/ij2jrsUmwCI)
2. [Function Pointers 1](https://www.youtube.com/watch?v=p4sDgQ-jao4)
3. [Function Pointers 2](https://youtu.be/BRsv3ZXoHto)
4. [Win32 API Documentation](https://learn.microsoft.com/en-us/windows/win32/api/)
5. [C fopen() Function](https://www.geeksforgeeks.org/c-fopen-function-with-examples/)
6. [Winpcap Documentation](https://www.winpcap.org/docs/docs_412/html/group__wpcapfunc.html#ga98f36e62c95c6ad81eaa8b2bbeb8f16e)
7. [CFF by OALabs](https://www.youtube.com/watch?v=ySrvgnsz3q0)
8. [Restoring CFF Obfuscated Code](https://www.youtube.com/watch?v=ySrvgnsz3q0)
9. [CFF Article](https://reverseengineering.stackexchange.com/questions/2221/what-is-a-control-flow-flattening-obfuscation-technique)
10. [fseek() C Function](https://www.tutorialspoint.com/c_standard_library/c_function_fseek.htm)
11. [fwrite() C Function](https://www.w3schools.com/c/ref_stdio_fwrite.php)
12. [fread() C Function](https://www.geeksforgeeks.org/fread-function-in-c/)
13. [Simple Function Hooking](https://www.youtube.com/watch?v=TxBGBz7FRyk)
14. [fgets() C Function](https://www.geeksforgeeks.org/fgets-gets-c-language/)
15. [strcspn() C Function](https://www.geeksforgeeks.org/strcspn-in-c/)
16. [x86 Memory Segmentation](https://en.wikipedia.org/wiki/X86_memory_segmentation)
17. [Flat Memory Model](https://en.wikipedia.org/wiki/Flat_memory_model)
18. [Applied Reverse Engineering: The Stack](https://revers.engineering/applied-re-the-stack/)
19. [Endianness](https://en.wikipedia.org/wiki/Endianness)
20. [2's Complement](https://en.wikipedia.org/wiki/Two%27s_complement)
--- 