#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "mylib.h"

void *my_malloc(size_t _size)
{
    void *_memory = dmalloc(_size);
    // void *_memory = malloc(_size);
    if (!_memory)
    {
        fprintf(stderr, "malloc error\n");
        exit(1);
    }
    return _memory;
}
void my_free(void *_memory)
{
    dfree(_memory);
    // free(_memory);
    _memory = NULL;
}
void *my_malloc_str(size_t _size)
{
    void *memory = my_malloc(_size);

    // c++のmapにmallocのポインターをキーにして保存。
    my_malloc_check_t *value = my_malloc(sizeof(my_malloc_check_t));
    value->length = _size;
    cpp_map_set(map_voidkey_kind, g_map_malloc_str, memory, value);

    return memory;
}
void my_free_str(void *_memory)
{
    if (!_memory)
        return;

    my_malloc_check_t *value = cpp_map_get(map_voidkey_kind, g_map_malloc_str, _memory);

    if (!value)
        return;

    // freeする前に文字列の長さ(strlen)とmallocサイズが一致するかチェック。文字列の結合しかしていない。
    int s_len = strlen((char *)_memory);
    if (s_len != value->length - 1)
    {
        fprintf(stderr, "error at my_free_str, %d(strlen) != %d(value->length-1)\n", s_len, value->length - 1);
        exit(1);
    }

    cpp_map_erase(map_voidkey_kind, g_map_malloc_str, _memory);

    my_free(_memory);

    my_free(value);
}
void *my_malloc_node(size_t _size)
{
    void *_memory = my_malloc(_size);

    // 追加mallocしてNode用のmapに追加
    my_malloc_check_t *value = my_malloc(sizeof(my_malloc_check_t));
    value->length = _size;
    cpp_map_set(map_voidkey_kind, g_map_malloc_debug_node, _memory, value);

    return _memory;
}
void my_free_node(void *_memory, int8_t free_env_var)
{
    // node->env_varをfreeするならfree_env_var=1;
    Node *node = (Node *)_memory;

    // run前のnodeならfreeしない
    if (!node->after_run)
    {
        return;
    }

    // return nodeならfreeしない（がenv_varを0にする)
    if (node->return_value)
    {
        node->env_var = 0;
        return;
    }

    // envにセットされたvarなら、envをfreeするときしかfreeしない
    if (node->env_var && !free_env_var)
    {
        return;
    }

    // freeするので意味はないがデバッグ用。
    node->freed = 1;

    // ポインターのアドレスがmallocしたnodeか（freeされていないか）をチェックする。
    my_malloc_check_t *value = cpp_map_get(map_voidkey_kind, g_map_malloc_debug_node, _memory);

    if (!value)
    {
        fprintf(stderr, "free check error at my_free_node\n");
        exit(1);
    }

    cpp_map_erase(map_voidkey_kind, g_map_malloc_debug_node, _memory);

    my_free(value);

    my_free(_memory);
}
void *my_malloc_env(size_t _size)
{
    void *_memory = my_malloc(_size);

    // 追加mallocしてEnv用のmapに追加
    my_malloc_check_t *value = my_malloc(sizeof(my_malloc_check_t));
    value->length = _size;
    cpp_map_set(map_voidkey_kind, g_map_malloc_debug_env, _memory, value);

    return _memory;
}
void my_free_env(void *_memory)
{
    // ポインターのアドレスがmallocしたenvか（freeされていないか）をチェックする。
    my_malloc_check_t *value = cpp_map_get(map_voidkey_kind, g_map_malloc_debug_env, _memory);

    if (!value)
    {
        fprintf(stderr, "free check error at my_free_env\n");
        exit(1);
    }

    cpp_map_erase(map_voidkey_kind, g_map_malloc_debug_env, _memory);

    my_free(value);

    my_free(_memory);
}
