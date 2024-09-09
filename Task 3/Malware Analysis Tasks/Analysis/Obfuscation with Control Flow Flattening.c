#include <stdio.h>

    void obfuscated_code() {
        int x = 0;
        int y = 5;
        int state = 0;

        while (state < 4) {
            switch (state) {
                case 0:
                    x = y + 2;
                    state = 1;
                    break;
                case 1:
                    y = x - 3;
                    state = 2;
                    break;
                case 2:
                    x = x + y;
                    state = 3;
                    break;
                case 3:
                    printf("x: %d, y: %d\n", x, y);
                    state = 4;
                    break;
            }
        }
    }

    int main() {
        obfuscated_code();
        return 0;
    } //result x = 11, y = 4