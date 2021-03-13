# QKumaScript

QKumaScript is an experimental scripting language using Bison.

## Quick Start

```console
// make to build
$ make

// run a test script
$ qks tests/a1.qks
```

You can see sample scripts under the tests folder.

## BNF Grammar

```EBNF
program  ::= statement_list
statement_list ::= statement*
statement ::= statement_no_semicolon ';'
           | "if" '(' exp ')' statement_list_paren else_part
           | ( "while" '(' exp | "for" '(' statement_no_semicolon ';' exp ';' statement_no_semicolon ) ')' statement_list_paren

statement_no_semicolon ::= ( ( "print" | "println" | "debug" | "return" | "var"? VAR '=' )? function_or_exp )?
statement_list_paren ::= '{' statement_list '}'
else_part ::= ( "else" statement_list_paren )?
function_or_exp ::= "function" '(' parameter? ( ',' parameter )* ')' statement_list_paren
           | exp
exp      ::= ( '!' | '-' | exp ( "**" | '*' | '/' | '%' | '+' | '-' | '<' | "==" | "&&" | "||" ) ) exp
           | '(' exp ')'
           | VAR ( '(' argument? ( ',' argument )* ')' )?
           | NUM
           | STRING
argument ::= exp
parameter ::= VAR
```
