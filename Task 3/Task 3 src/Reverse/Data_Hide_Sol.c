#include <stdio.h>
#include <string.h>
char output_data[] = {0x1d,0x30,0x39,0x39,0x3a,0x55};
void print_output_data(){
    char *data = output_data;
    int cnt = 0;
for (char* item = output_data;cnt!=sizeof(output_data);item++,cnt++)
    *item = (char)((long)(*item)^0x55555555);

printf("%s",output_data);
}
void main(){
    printf("Encoded Message: ");
    printf("%s",output_data);
    printf("\nDecoded Message: ");
    print_output_data();
    return;
}