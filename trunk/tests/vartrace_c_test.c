#include <stdio.h>

#include "vartrace/vartrace_c.h"

int main(void) 
{
    int i = 10;
    unsigned buffer[256];
    VT_TraceHandle trace = VT_create(128);

    VT_logInt(trace, 1, i);
    
    printf("Is empty: %d\n", VT_isEmpty(trace));
    printf("Error flags: %d\n", VT_errorFlags(trace));
    printf("Dump size: %d\n", VT_dump(trace, buffer, 256));

    printf("\nMessage\n");
    printf("stamp: %d\n", buffer[0]);
    printf("data size: %d\n", *( ((unsigned short* )buffer) + 2));
    printf("message id: %d\n", *( ((unsigned char* )buffer) + 6));
    printf("data type id: %d\n", *( ((unsigned char* )buffer) + 7));
    printf("value: %d\n", buffer[2]);

    VT_destroy(trace);
    
    return 0;
}
