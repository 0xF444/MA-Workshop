# Programming Task: GCD.c
This program attempts to calculate the gcd (Greatest Common Divisor) of two numbers using an iterative approach.
```c
#include <stdio.h>
int GCD(int a, int b){
    int r,temp;
if(a<b){
    temp = b;
    b = a;
    a = temp;
}
    r = a % b;
    if (r ==0) return b;
    while(r){
    a = b;
    b = r;
    r = a % b;
    }
    return b;
}
int main(int argc, char* argv[]){
int a,b;
printf("Please enter a,b respectively.\n");
scanf("%d%d",&a,&b);
printf("Operating: gcd(%d,%d)\n",a,b);
printf("Result = %d\n",GCD(a,b));
}
```
## Output
Using `a = 66528, b = 52920` as a test case:

![Pasted image 20240807192004](Assets/Pasted%20image%2020240807192004.png)
# Reversing Task
The program given was as following:
```c
#include <stdio.h>
#include <ctype.h>
#include <string.h>
int main()
{
    char message[99999];
    printf("Enter a string: ");
    fgets(message, sizeof(message), stdin);

    for (int i = 0; message[i] != '\0'; i++) {

        if (isupper((unsigned char)message[i])) {
            message[i] = tolower((unsigned char)message[i]);
        }
    }
    printf("Lowercase string: %s\n", message);
    return 0;
}
```
The program iterates over each character individually in a given `message` string variable and uses the C library function `isupper()` to check if its an uppercase character, if so then it converts it to a lowercase character using the `tolower()` function.

![Pasted image 20240807193645](Assets/Pasted%20image%2020240807193645.png)

## Solution
Since the program is based on conditions for its operation, we can simply reverse its operations by either using the functions that does the exact opposite, or we can negate the first condition.
```c
#include <stdio.h>
#include <ctype.h>
#include <string.h>
int main()
{
    char message[99999];
    printf("Enter a string: ");
    fgets(message, sizeof(message), stdin);
    for (int i = 0; message[i] != '\0'; i++) {
        if (islower(message[i])) {
            message[i] = toupper(message[i]);
        }
    }
    printf("Uppercase string: %s\n", message);
    return 0;
}
//Second Approach
#include <stdio.h>
#include <ctype.h>
#include <string.h>
int main()
{
    char message[99999];
    printf("Enter a string: ");
    fgets(message, sizeof(message), stdin);
    for (int i = 0; message[i] != '\0'; i++) {
        if (!(isupper(message[i]))) {
            message[i] = toupper(message[i]);
        }
    }
    printf("Uppercase string: %s\n", message);
    return 0;
}
```

Both yield the same output: 

![Pasted image 20240807194548](Assets/Pasted%20image%2020240807194548.png)

---
