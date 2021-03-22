%{
#include <string.h>
#include "node.h"
uint64_t line_no = 1;
%}

%option noyywrap
%option nounput
%option noinput

%%
[ \t]		{ ; }
^\/\/.*$    { ; }
\n			{ line_no++; }
if			{ return (IF); }
else		{ return (ELSE); }
while		{ return (WHILE); }
for 		{ return (FOR); }
print		{ return (PRINT); }
println		{ return (PRINTLN); }
var 		{ return (VARSTR); }
debug       { return (DEBUG); }
function    { return (FUNCTION); }
return      { return (RETURN); }
"=="		{ return (EQ); }
"&&"		{ return (AND); }
"||"		{ return (OR); }
"**"		{ return (POWER); }
[A-Za-z][A-Za-z0-9]*    { yylval.str = strdup(yytext);
                            return (VAR); }
[0-9]+      { yylval.val = atoi(yytext);
                return (NUM); }
\"[^"]*\"   { yytext[yyleng-1] = '\0';
                yylval.str = strdup(&yytext[1]);
                return (STRING); }
.           { return (yytext[0]); }
%%
