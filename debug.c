#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "mylib.h"
#include <assert.h>

char *sn(Node *node, Env *env)
{
    // デバッグコンソール用 sn(node, env)
    return node_tostring(node, env, 0);
}
char *pn(Node *node, Env *env)
{
    // デバッグコンソール用 pn(node, env)
    print_node(node, env);

    if (node->line_num)
        printf("line: %" PRIu64 "\n", node->line_num);

    return 0;
}
char *pe(Env *env)
{
    // デバッグコンソール用 pe(env)
    uint64_t depth = 0;
    while (env)
    {
        printf("pe reverse depth = %" PRIu64 "\n", depth);
        depth++;

        cpp_map_list_keys(map_stringkey_kind, env->map_void);
        env = env->parent;
    }
    return 0;
}
void debug_check_map_node(Node *node)
{
    // デバッグチェック。nodeがmapに存在しなければエラー。
    my_malloc_check_t *value = cpp_map_get(map_voidkey_kind, g_map_malloc_debug_node, node);
    if (!value)
    {
        fprintf(stderr, "error: node %p is freed at debug_check_map_node\n", node);
        exit(1);
    }
}
