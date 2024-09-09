#include <stdio.h>
#include <string.h>

char hidden_data[] = {
    0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x00
};

void print_hidden_data() {
    char *data = (char *)((long)hidden_data ^ 0x55555555);
    printf("Hidden data: %s\n", data);
}

int main() {
    print_hidden_data();
    return 0;
}