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
