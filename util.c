#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "node.h"
#include "mylib.h"

int64_t my_power(int64_t base, int64_t index)
{
    int64_t sum = 1;
    for (int64_t i = 0; i < index; i++)
    {
        sum *= base;
    }
    return sum;
}

void yyerror(char const *s)
{
    fprintf(stderr, "%s at line %" PRIu64 ".\n", s, line_no);
}

Env *make_env(Env *env_in)
{
    Env *new_env = my_malloc_env(sizeof(Env));

    new_env->parent = env_in;
    new_env->map_void = cpp_map_new(map_stringkey_kind);

    return new_env;
}
void free_env(Env *env_in)
{
    cpp_map_delete(map_stringkey_node_kind, env_in->map_void);
    my_free_env(env_in);
}
