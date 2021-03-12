%{
#include <stdio.h>
#include "node.h"
%}

%union {
	int64_t val;
	char *str;
	Node *node;
}

%token <str> VAR
%token <val> NUM
%token <str> STRING
%token PRINT IF ELSE WHILE EQ VARSTR PRINTLN FOR DEBUG FUNCTION POWER RETURN AND OR

%type <node> statement_list statement statement_no_semicolon statement_list_paren else_part exp parameter_list parameter argument_list argument function_or_exp

%left OR
%left AND
%left EQ
%left '<'
%left '+' '-'
%left '*' '/' '%'
%right POWER
%right '!' UMINUS

%start program
%%
program
	: statement_list {
		run_start($1); }
	;

statement_list
	: statement_list statement { $$ = make_node( Stmt_List_Node, NULL, $1, $2, NULL, 0,NULL, line_no, NULL); }
	| { $$ = NULL; }
	;

statement
	: statement_no_semicolon ';' { $$ = $1; }
	| IF '(' exp ')' statement_list_paren else_part { $$ = make_node( If_Node, $3, $5, $6, NULL, 0, NULL, line_no, NULL); }
	| WHILE '(' exp ')' statement_list_paren { $$ = make_node( While_Node, $3, NULL, NULL, $5, 0, NULL, line_no, NULL); }
	| FOR '(' statement_no_semicolon ';' exp ';' statement_no_semicolon ')' statement_list_paren { $$ = make_node( For_Node, $5, $3, $7, $9, 0, NULL, line_no, NULL); }
	;

statement_no_semicolon
	: PRINT function_or_exp { $$ = make_node( Print_Val_Node, NULL, $2, NULL, NULL, 0, NULL, line_no, NULL); }
	| PRINTLN { $$ = make_node( Println_Node, NULL, NULL, NULL, NULL, 0, NULL, line_no, NULL); }
	| DEBUG function_or_exp { $$ = make_node( Debug_Node, NULL, $2, NULL, NULL, 0, NULL, line_no, NULL); }
	| RETURN function_or_exp { $$ = make_node( Return_Node, NULL, NULL, NULL, $2, 0, NULL, line_no, NULL); }
	| VAR '=' function_or_exp { $$ = make_node( Assign_Node, NULL, $3, NULL, NULL, 0, $1, line_no, NULL); }
	| VARSTR VAR '=' function_or_exp { $$ = make_node( Assign_Var_Node, NULL, $4, NULL, NULL, 0, $2, line_no, NULL); }
	| function_or_exp
	| { $$ = NULL; }
	;

statement_list_paren
	: '{' statement_list '}' { $$=$2; }
	;

else_part
	: ELSE statement_list_paren { $$=$2; }
	| { $$ = NULL; }
	;

function_or_exp
	: FUNCTION '(' parameter_list ')' statement_list_paren { $$= make_node( Function_Node, NULL, $3, NULL, $5, 0, NULL, line_no, NULL); }
	| exp
	;

exp
	: '!' exp { $$ = make_node( Not_Node, NULL, $2, NULL, NULL, 0, NULL, line_no, NULL); }
	| '-' exp %prec UMINUS { $$ = make_node( Uminus_Node, NULL, $2, NULL, NULL, 0, NULL, line_no, NULL); }
	| exp '+' exp { $$ = make_node( Add_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| exp '-' exp { $$ = make_node( Sub_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| exp '<' exp { $$ = make_node( Less_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| exp OR exp { $$ = make_node( Or_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| exp AND exp { $$ = make_node( And_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| exp EQ exp { $$ = make_node( Eq_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| exp POWER exp { $$ = make_node( Power_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| exp '*' exp { $$ = make_node( Mul_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| exp '/' exp { $$ = make_node( Div_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| exp '%' exp { $$ = make_node( Mod_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| '(' exp ')' { $$ = $2; }
	| VAR '(' argument_list ')' { $$ = make_node( Call_Node, NULL, $3, NULL, NULL, 0, $1, line_no, NULL); }
	| VAR { $$ = make_node( Var_Node, NULL, NULL, NULL, NULL, 0, $1, line_no, NULL); }
	| NUM { $$ = make_node( Num_Node, NULL, NULL, NULL, NULL, $1, NULL, line_no, NULL); }
	| STRING { $$ = make_node( String_Node, NULL, NULL, NULL, NULL, 0, $1, line_no, NULL); }
	;

argument_list
	: argument_list ',' argument { $$ = make_node( Parameter_List_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| argument
	| { $$ = NULL; }
	;

argument
	: exp { $$ = make_node( Parameter_Node, NULL, NULL, NULL, $1, 0, NULL, line_no, NULL); }
	;

parameter_list
	: parameter_list ',' parameter { $$ = make_node( Parameter_List_Node, NULL, $1, $3, NULL, 0, NULL, line_no, NULL); }
	| parameter
	| { $$ = NULL; }
	;

parameter
	: VAR { $$ = make_node( Parameter_Node, NULL, NULL, NULL, NULL, 0, $1, line_no, NULL); }
	;

%%
