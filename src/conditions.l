/* Lex compound logical expressions. */

%{
#include "parse.h"
#include "stringset.h"
#include "varmap.h"

#include "conditions.tab.h"

// TODO: this should probably go in the .y file instead.
parser_t* g_ctx = NULL;
%}

%option noyywrap
%x STRING

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
{VAR}   { yylval.varname_val = yytext; return VAR; }

"\""          { BEGIN(STRING); }
<STRING>[^"]* { yylval.str_val = yytext; return STR; } 
<STRING>"\""  { BEGIN(INITIAL); }

{NL}    {
          int len = (yytext[1] == '\r') ? strlen(yytext + 2) : strlen(yytext + 1);
          if (len > g_ctx->indents[g_ctx->indent_level]) {
            g_ctx->indents[++g_ctx->indent_level] = len;
            return INDENT;
          }
          else if (len < g_ctx->indents[g_ctx->indent_level]) {
            yyless(0);
            --g_ctx->indent_level;
            return DEDENT;
          }
          else {
            return NEWLINE;
          }
        }

.       { /* ignore all other characters */ }

%%

bool_t fetchdeps_parser_parse(parser_t* ctx)
{
  g_ctx = ctx;
  yyin = ctx->f;
  yyparse();
  return 1;
}

