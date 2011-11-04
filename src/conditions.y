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

%token LT
%token LE
%token GT
%token GE
%token EQ
%token NE

%token INDENT
%token DEDENT
%token COLON
%token NEWLINE

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


value:
    int_value
  | str_value
  ;


relational_clause:
    VAR value
  | VAR EQ value
  | VAR NE value
  | VAR LT value
  | VAR LE value
  | VAR GT value
  | VAR GE value
  ;


negatable_clause:
    relational_clause
  | NOT relational_clause
  ;


logical_clause:
    negatable_clause
  | logical_clause AND negatable_clause
  | logical_clause OR negatable_clause
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

