#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "mylib.h"

char *my_strcat(const char *args, ...)
{
    size_t len = 0;
    va_list ap;
    va_start(ap, args);

    for (const char *arg = args; arg != VEND; arg = va_arg(ap, const char *))
    {
        if (arg == 0)
            continue;

        len += strlen(arg);
    }
    va_end(ap);

    char *ret = (char *)my_malloc_str(len + 1);
    if (!ret)
    {
        fprintf(stderr, "my_malloc_str error\n");
        exit(1);
    }
    char *ret_ptr = ret;

    va_start(ap, args);
    for (const char *arg = args; arg != VEND; arg = va_arg(ap, const char *))
    {
        if (arg == 0)
            continue;

        strcpy(ret_ptr, arg);
        ret_ptr += strlen(arg);
    }
    va_end(ap);

    *(ret + len) = '\0';
    assert(strlen(ret) == len);

    return ret;
}
char *space_repeat(size_t n)
{
    char *a = (char *)my_malloc_str(n * 2 + 1);
    size_t i;
    for (i = 0; i < n * 2; i++)
    {
        *(a + i) = ' ';
    }
    *(a + i) = '\0';
    assert(strlen(a) == n * 2);
    return a;
}
