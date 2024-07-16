#include <stdio.h>

void function1() {
    printf("This is function 1\n");
}

void function2() {
    printf("This is function 2\n");
}

void function3() {
    printf("This is function 3\n");
}

int main() {
    void (*func_ptr)();
    char choice;

    printf("Enter 1, 2, or 3: ");
    scanf(" %c", &choice);

    if (choice == '1') {
        func_ptr = function1;
    } else if (choice == '2') {
        func_ptr = function2;
    } else if (choice == '3') {
        func_ptr = function3;
    } else {
        printf("Invalid choice\n");
        return 1;
    }

    func_ptr();
    return 0;
}
