/* Lex compound logical expressions. */

%{
#include "errors.h"
#include "parse.h"
#include "stringset.h"
#include "varmap.h"

#include "conditions.tab.h"

#define YY_USER_ACTION { \
  yylloc.first_line = yylloc.last_line = yylineno; \
  yylloc.first_column = yycolumn; \
  yylloc.last_column = yycolumn + yyleng - 1; \
  yycolumn += yyleng; \
}
#define YY_NO_INPUT 1

// Forward declaration.
extern int yyparse(stringset_t* results);

parser_t* g_ctx = NULL;
int yycolumn = 1;
%}

%option noyywrap
%option nounput
%option yylineno

%x STRING

VAR   [a-zA-Z_][a-zA-Z0-9_]*
URL   ("http://"|"https://"|"file://")[a-zA-Z0-9./#:\-?=_]+

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

{NL}$   { /* ignore blank lines */ }

{NL}    {
          int len = (yytext[1] == '\r') ? strlen(yytext + 2) : strlen(yytext + 1);

          yylloc.first_column = 1;
          yylloc.last_column = len;
          yycolumn = len + 1;

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

bool_t fetchdeps_parser_parse(parser_t* ctx, stringset_t* results)
{
  g_ctx = ctx;
  yyin = ctx->f;
  
  if (yyparse(results) != 0)
    goto failure;
  
  return 1;

failure:
  fetchdeps_errors_set(ERR_PARSE);
  return 0;
}

