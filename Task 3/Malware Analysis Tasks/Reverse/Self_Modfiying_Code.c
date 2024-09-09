#include <stdio.h>
#include <string.h>
#include <windows.h>

void self_modify(char *code, int len) {
    DWORD oldProtect;
    VirtualProtect(code, len, PAGE_EXECUTE_READWRITE, &oldProtect);
    for (int i = 0; i < len; i++) {
        code[i] ^= 0xFF;
    }
    VirtualProtect(code, len, oldProtect, &oldProtect);
}

int main() {
    char code[] = "\x55\x48\x89\xE5\xB8\x01\x00\x00\x00\x5D\xC3";
    self_modify(code, sizeof(code) - 1);
    printf("After modification: ");
    for (int i = 0; i < sizeof(code) - 1; i++) {
        printf("\\x%02x", (unsigned char)code[i]);
    }
    printf("\n");

    return 0;
}
