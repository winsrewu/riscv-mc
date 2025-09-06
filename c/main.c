#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "include/mcsyscalls.c"

int main()
{
    char *str = "{flag: true}";
    bool result = run_if(str, strlen(str));

    printf("The result is %d\n", result);

    return 0;
}