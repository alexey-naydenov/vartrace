#include <stdio.h>

#include "vartrace/vartrace_c.h"

int main(void) 
{
    unsigned char buffer[256];
    VT_TraceHandle trace = VT_create(128);
    
    printf("Is empty: %d\n", VT_isEmpty(trace));
    printf("Error flags: %d\n", VT_errorFlags(trace));
    printf("Dump size: %d\n", VT_dump(trace, buffer, 256));

    VT_destroy(trace);
    
    return 0;
}
