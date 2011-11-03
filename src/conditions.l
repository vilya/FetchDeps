/* Lex compound logical expressions. */

%{
#include "conditions.tab.h"

int indents[100] = { 0 };
int curr_indent = 0;
%}

%option noyywrap
%x STRING
%x DEDENTING

INT   [0-9]+
VAR   [a-zA-Z_][a-zA-Z0-9_]*
URL   ("http://"|"https://"|"file://")[a-zA-Z0-9./#:\-]+

NL    \n\r?" "*

%%

"and"   { return AND; }
"or"    { return OR; }

"not"   { return NOT; }

"="     { return EQ; }
"!="    { return NE; }
"<="    { return LE; }
"<"     { return LT; }
">="    { return GE; }
">"     { return GT; }

","     { return COMMA; }
":"     { return COLON; }

{URL}   { yylval.url_val = yytext; return URL; }
{INT}   { yylval.int_val = atoi(yytext); return INT; }
{VAR}   { yylval.var_val = yytext; return VAR; }

"\""          { BEGIN(STRING); }
<STRING>[^"]* { yylval.str_val = yytext; return STR; } 
<STRING>"\""  { BEGIN(INITIAL); }

{NL}    {
          int len = (yytext[1] == '\r') ? strlen(yytext + 2) : strlen(yytext + 1);
          if (len > indents[curr_indent]) {
            indents[++curr_indent] = len;
            return INDENT;
          }
          else if (len < indents[curr_indent]) {
            yyless(0);
            --curr_indent;
            return DEDENT;
          }
          else {
            return NEWLINE;
          }
        }

.       { /* ignore all other characters */ }

%%

int main(int argc, char** argv)
{
  memset(indents, 0, sizeof(indents));
  if (argc > 1)
    yyin = fopen(argv[1], "r");
  else
    yyin = stdin;

  yyparse();

  printf("\n");
  if (yyin != stdin)
    fclose(yyin);

  return 0;
}
