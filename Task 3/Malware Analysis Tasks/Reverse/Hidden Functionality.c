#include <stdio.h>
#include <string.h>

void hidden_feature(char *input) {
    char key[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE};
    char secret[] = {0x7F, 0x49, 0x2F, 0x12, 0xA3, 0xDF, 0x4B}; 
    
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

int main() {
    char input[20];
    printf("Enter command: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0; 
    hidden_feature(input);
    return 0;
}
