%{
#include "yynfa.hxx"
#include <iostream>

extern int yylex (void);
void yyerror (char *s);

%}

%token TOK_STATE, TOK_ID, TOK_ARROW

%union {
std::string* str;
char ch;
}

%%

start : state start                             {std::clog << "start: state start" << std::endl;}
| state                                         {std::clog << "start: state" << std::endl;}
;

state : TOK_STATE TOK_ID '{' delta_list '}'	{std::clog << "state: TOK_STATE TOK_ID '{' delta_list '}'" << std::endl;}
;

delta_list : delta delta_list                   {std::clog << "delta_list: delta delta_list" << std::endl;}
| delta                                         {std::clog << "delta_list: delta" << std::endl;}
| /* empty */                                   {std::clog << "delta_list: /* empty */" << std::endl;}
;

delta : TOK_ID TOK_ARROW '{' state_list '}'	{std::clog << "delta: TOK_ID TOK_ARROW '{' state_list '}'" << std::endl;}
;

state_list :
  TOK_ID ',' state_list				{std::clog << "state_list: TOK_ID ',' state_list" << std::endl;}  
| TOK_ID state_list                             {std::clog << "state_list: TOK_ID state_list" << std::endl;} 
| TOK_ID                                        {std::clog << "state_list: TOK_ID" << std::endl;}
| /* empty */                                   {std::clog << "state_list: /* empty */" << std::endl;}
;

%%

void yyerror (char *s)
{		
    fprintf (stderr, "%s\n", s);
}
