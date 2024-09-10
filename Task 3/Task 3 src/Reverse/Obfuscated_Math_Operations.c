#include <stdio.h>

int obfuscated_math(int a, int b) {
    int result = 0;
    while (a != 0) {
        if (a & 1) {
            result ^= b; //Obfuscate if x is odd.
        }
        b <<= 1;
        a >>= 1;
    }
    return result;
}

int main() {
    int x,y;
    scanf("%d%d",&x,&y);
    printf("Result: %d\n", obfuscated_math(x, y));
    return 0;
}
