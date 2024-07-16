#include <stdio.h>
#include <string.h>

void encode(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        str[i] += 3;
    }
}

void decode(char *str) {
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
