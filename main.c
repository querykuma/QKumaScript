#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "mylib.h"
#include <assert.h>

void *g_map_malloc_str;
void *g_map_malloc_debug_node;
void *g_map_malloc_debug_env;
Env *g_env;

extern FILE *yyin, *yyout;

int main(int argc, char **argv)
{
	const char *filename = "a.qks";
	FILE *fp = NULL;

	if (argc >= 2)
	{
		int sc = strcmp(argv[1], "-d");
		if (sc == 0)
		{
			yydebug = 1;
			fp = fopen(argc >= 3 ? argv[2] : filename, "r");
		}
		else
		{
			fp = fopen(argv[1], "r");
		}
	}
	else
	{
		fp = fopen(filename, "r");
	}
	if (!fp)
	{
		printf("can't open file\n");
		return -1;
	}
	yyin = fp;

	g_map_malloc_str = cpp_map_new(map_voidkey_kind);
	g_map_malloc_debug_node = cpp_map_new(map_voidkey_kind);
	g_map_malloc_debug_env = cpp_map_new(map_voidkey_kind);

	g_env = make_env(NULL);
	g_env->map_void = cpp_map_new(map_stringkey_kind);

	yyparse();

	// mallocチェック
	size_t g_map_malloc_str_size = cpp_map_size(map_voidkey_kind, g_map_malloc_str);
	printf("g_map_malloc_str_size = %zu\n", g_map_malloc_str_size);

	size_t g_map_malloc_debug_node_size = cpp_map_size(map_voidkey_kind, g_map_malloc_debug_node);
	printf("g_map_malloc_debug_node_size = %zu\n", g_map_malloc_debug_node_size);

	size_t g_map_malloc_debug_env_size = cpp_map_size(map_voidkey_kind, g_map_malloc_debug_env);
	printf("g_map_malloc_debug_env_size = %zu\n", g_map_malloc_debug_env_size);

	// print_d_malloc_stat();
	// print_d_malloc_controls();
}
