%{
//#include "yynfa.hxx"
#include "nfa.hxx"
#include <iostream>

extern int yylex (void);

void yyerror (char *s);

%}

%token TOK_STATE, TOK_ID, TOK_ARROW

%type <StateAndDeltaPairT> state
%type <StateDeltaT*> delta_list
%type <LetterAndSetOfStatesPairT> delta
%type <SetOfStatesT*> state_list
%type <YYSTYPE> TOK_ID

%union {
std::string* str;
void* ptr;
char ch;
}

%%

start : state start                             { nfa->insert($1); }
| state                                         { nfa->insert($1); }
;

state : TOK_STATE TOK_ID '{' delta_list '}'	{ $$ = make_pair(*($2.str),*$4); }
;

delta_list : delta delta_list                   { $2->insert($1); $$ = $2; }
| delta                                         { $$ = new StateDeltaT; $$->insert($1); }
| /* empty */                                   { $$ = new StateDeltaT; }
;

delta : TOK_ID TOK_ARROW '{' state_list '}'	{ $$ = make_pair(*($1.str),*$4); }
;

state_list :
  TOK_ID ',' state_list				{ $3->insert(*($1.str)); $$ = $3; }  
| TOK_ID state_list                             { $2->insert(*($1,str))); $$ = $2; } 
| TOK_ID                                        { $$ = new SetOfStatesT; $$->insert(*($1.str)); }
| /* empty */                                   { $$ = new SetOfStatesT; }
;

%%

void yyerror (char *s)
{		
    fprintf (stderr, "%s\n", s);
}
