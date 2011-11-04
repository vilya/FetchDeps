/* Parse compound logical expressions. */

%{
#include "parse.h"
#include <stdio.h>

int yylex();
void yyerror(const char* s);

extern ParserContext* gContext;
%}

%union {
  int bool_val;
  int int_val;
  char* str_val;
  char* var_val;
  char* url_val;
}

%token <var_val> VAR
%token <int_val> INT
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

%type <str_val> str_value
%type <int_val> int_value

%error-verbose

%%

start:
  block
;


str_value:
    STR
  | str_value COMMA STR
  ;


int_value:
    INT
  | int_value COMMA INT
  ;


int_relation:
    VAR int_value
  | VAR NOT int_value
  | VAR EQ int_value
  | VAR NE int_value
  ;


str_relation:
    VAR str_value
  | VAR NOT str_value
  | VAR EQ str_value
  | VAR NE str_value
  ;


relation:
    int_relation
  | str_relation
  ;


logical_clause:
    relation
  | logical_clause AND relation
  | logical_clause OR relation
  ;


statement: /* empty */
  | URL { printf("URL: %s\n", $1); }
  | logical_clause COLON INDENT block DEDENT
  ;

block:
    statement
  | block NEWLINE statement


%%

void yyerror(const char* msg)
{
  fprintf(stderr, "%s\n", msg);
}

