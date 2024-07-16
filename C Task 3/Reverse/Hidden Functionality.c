#include <stdio.h>
#include <string.h>

void hidden_feature(char *input) {
    if (strcmp(input, "unlock") == 0) {
        printf("Secret feature activated!\n");
    } else {
        printf("Access denied.\n");
    }
}

int main() {
    char input[20];
    printf("Enter command: ");
    gets(input);
    hidden_feature(input);
    return 0;
}
