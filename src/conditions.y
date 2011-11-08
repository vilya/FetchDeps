/* Parse compound logical expressions. */

%{
#include "common.h"
#include "parse.h"
#include "stringset.h"
#include "varmap.h"
#include <stdio.h>

int yylex();
void yyerror(stringset_t* /*ignored*/, const char* s);

extern parser_t* g_ctx;
%}

%union {
  int bool_val;
  char* str_val;
  char* varname_val;
  char* url_val;
  stringset_t* strset_val;
  stringset_t* urlset_val;
}

%token <varname_val> VAR
%token <str_val> STR
%token <url_val> URL

%token COMMA

%token AND
%token OR
%token NOT

%token EQ
%token NE

%token INDENT
%token DEDENT
%token COLON
%token NEWLINE

%type <urlset_val> start
%type <strset_val> str_value
%type <strset_val> var_value
%type <bool_val> relation
%type <bool_val> condition
%type <urlset_val> statement
%type <urlset_val> block;

%error-verbose
%locations
%parse-param {stringset_t* parse_results} 

%%

start:
  block   { if (!fetchdeps_stringset_add_all(parse_results, $1)) {
              yyerror(parse_results, "failed to add URLs to result set");
              YYERROR;
            }
            /*fetchdeps_stringset_free($1);*/ }
;


str_value:
    STR                   { $$ = fetchdeps_stringset_new_single($1);
                            if (!$$) {
                              yyerror(parse_results, "failed to create new string literal");
                              YYERROR;
                            } }
  | str_value COMMA STR   { $$ = $1;
                            if (!fetchdeps_stringset_add($$, $3)) {
                              yyerror(parse_results, "failed to add literal string to string value");
                              YYERROR;
                            } }
  ;


var_value:
    VAR     { $$ = fetchdeps_varmap_get(g_ctx->vars, $1);
              if (!$$) {
                yyerror(parse_results, "unknown variable\n");
                YYERROR;
              } }
  ;


relation:
    var_value str_value       { $$ = fetchdeps_stringset_contains_any($1, $2);
                                fetchdeps_stringset_free($2); }
  | var_value NOT str_value   { $$ = !fetchdeps_stringset_contains_any($1, $3);
                                fetchdeps_stringset_free($3); }
  | var_value EQ str_value    { $$ = fetchdeps_stringset_contains_any($1, $3);
                                fetchdeps_stringset_free($3); }
  | var_value NE str_value    { $$ = !fetchdeps_stringset_contains_any($1, $3);
                                fetchdeps_stringset_free($3); }
  ;


condition:
    relation                { $$ = $1; }
  | condition AND relation  { $$ = $1 && $3; }
  | condition OR relation   { $$ = $1 || $3; }
  ;


statement: /* empty */                  { $$ = fetchdeps_stringset_new();
                                          if (!$$) {
                                            yyerror(parse_results, "failed to allocate empty statement");
                                            YYERROR;
                                          } }
  | URL                                 { $$ = fetchdeps_stringset_new_single($1);
                                          if (!$$) {
                                            yyerror(parse_results, "failed to allocate URL");
                                            YYERROR;
                                          } }
  | condition COLON INDENT block DEDENT { if ($1) {
                                            $$ = $4;
                                          }
                                          else {
                                            fetchdeps_stringset_free($4);
                                            $$ = fetchdeps_stringset_new();
                                            if (!$$) {
                                              yyerror(parse_results, "failed to allocate empty statement for ignored conditional block");
                                              YYERROR;
                                            }
                                          } }
  ;

block:
    statement                 { $$ = $1; }
  | block NEWLINE statement   { $$ = $1;
                                if (!fetchdeps_stringset_add_all($$, $3)) {
                                  yyerror(parse_results, "failed to add URLs from conditional section");
                                  YYERROR;
                                }
                                fetchdeps_stringset_free($3); }


%%

void yyerror(stringset_t* ignored, const char* msg)
{
  fprintf(stderr, "[line %d, cols %d - %d] %s\n",
          yylloc.first_line, yylloc.first_column, yylloc.last_column, msg);
}

