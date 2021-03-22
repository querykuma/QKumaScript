#ifndef NODE_H
#define NODE_H
#include <stdint.h>
typedef enum _Node_Type
{
	Stmt_List_Node,
	Print_Val_Node,
	Println_Node,
	Debug_Node,
	If_Node,
	While_Node,
	For_Node,
	Assign_Node,
	Assign_Var_Node,
	Var_Node,
	Num_Node,
	String_Node,
	Add_Node,
	Sub_Node,
	Mul_Node,
	Div_Node,
	Mod_Node,
	Less_Node,
	Eq_Node,
	Or_Node,
	And_Node,
	Power_Node,
	Not_Node,
	Function_Node,
	Call_Node,
	Return_Node,
	Closure_Node,
	Parameter_List_Node,
	Parameter_Node,
	Uminus_Node
} Node_Type;

typedef struct _Env Env;

typedef struct _Node
{
	Node_Type type;
	struct _Node *cond, *left, *right, *body;
	int64_t ivalue;
	char *svalue;
	uint64_t line_num;
	Env *env;
	struct _Node *parent;
	uint8_t return_value; //初期値0で1のとき関数からreturnする。
	uint8_t after_run;	  //run前のコードは0でrun後に作成したNodeは1。
	uint8_t env_var;	  //初期値0で、envに割り当てた変数なら1。
	uint8_t freed;		  //デバッグ用の変数。初期化0でfreeされたら1。
} Node;

typedef struct _Env
{
	Env *parent;
	void *map_void;
} Env;

// node.c
Node *make_node(Node_Type type, Node *cond, Node *left, Node *right, Node *body, int64_t ivalue, char *svalue, int64_t line_num, Env *env);
void run_start(Node *node);
Node *run(Node *node, Env *env);
void run_exit(Node *node, char *msg);
char *node_tostring(Node *node, Env *env, int depth);
void print_node(Node *node, Env *env);

#include "y.tab.h"

// util.c
void yyerror(char const *s);

// y.tab.c
int yylex();
int yyparse();

// lex.yy.c
extern uint64_t line_no;

// main.c
extern Env *g_env;

#endif
