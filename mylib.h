#include "node.h"

typedef enum
{
    map_voidkey_kind,
    map_stringkey_kind,
    map_stringkey_node_kind,
    map_count_kind
} map_kind;

typedef struct my_malloc_check
{
    int length;
} my_malloc_check_t;

// main.c
extern void *g_map_malloc_str;
extern void *g_map_malloc_debug_node;
extern void *g_map_malloc_debug_env;

// malloc.c
void *my_malloc_str(size_t _size);
void my_free_str(void *_memory);
void *my_malloc_node(size_t _size);
void my_free_node(void *_memory, int8_t free_env_var);
void *my_malloc_env(size_t _size);
void my_free_env(void *_memory);

// str.c
#define VEND ((const char *)-1)
char *my_strcat(const char *args, ...);
char *space_repeat(size_t n);

// map.cc
void cpp_map_set(map_kind mkind, void *map_void, void *key, void *value);
void *cpp_map_get(map_kind mkind, void *map_void, void *key);
void cpp_map_erase(map_kind mkind, void *map_void, void *key);
void cpp_map_list_keys(map_kind mkind, void *map_void);
size_t cpp_map_size(map_kind mkind, void *map_void);
void *cpp_map_new(map_kind mkind);
void cpp_map_delete(map_kind mkind, void *map_void);

// debug.c
void debug_check_map_node(Node *node);

// util.c
int64_t my_power(int64_t base, int64_t index);
Env *make_env(Env *env_in);
void free_env(Env *env_in);

// d_malloc.c
void *dmalloc(size_t size);
void dfree(void *_memory);
void print_d_malloc_stat();
void print_d_malloc_controls();
