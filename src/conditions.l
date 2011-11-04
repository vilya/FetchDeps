/* Lex compound logical expressions. */

%{
#include "conditions.tab.h"
#include "parse.h"

// TODO: this should probably go in the .y file instead.
ParserContext* gContext = NULL;
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
          if (len > gContext->indents[gContext->indentLevel]) {
            gContext->indents[++gContext->indentLevel] = len;
            return INDENT;
          }
          else if (len < gContext->indents[gContext->indentLevel]) {
            yyless(0);
            --gContext->indentLevel;
            return DEDENT;
          }
          else {
            return NEWLINE;
          }
        }

.       { /* ignore all other characters */ }

%%

bool_t Parse(ParserContext* ctx)
{
  gContext = ctx;
  yyin = ctx->f;
  yyparse();
  return 1;
}

