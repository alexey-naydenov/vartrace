#include <stdio.h>

#include "vartrace/vartrace_c.h"

int main(void) 
{
#ifdef __cplusplus
    printf("Error c++ mode\n");
#endif
    
    printf("ok %d\n", VT_create());
    return 0;
}
