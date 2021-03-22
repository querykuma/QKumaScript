#include <stdio.h>
#include <stdint.h>
#include "mylib.h"

int main()
{
    int *a = dmalloc(sizeof(int));
    *a = 99;
    int *b = dmalloc(sizeof(int));
    *b = -3;
    uint64_t *c = dmalloc(sizeof(uint64_t));
    *c = 5;
    dfree(a);
    dfree(b);
    a = dmalloc(sizeof(int));
    *a = 999;
    a = dmalloc(sizeof(int));
    *a = 999;
    dfree(c);
    char *d = dmalloc(100);
    *d = 'a';

    print_d_malloc_stat();
    print_d_malloc_controls();
    return 0;
}
