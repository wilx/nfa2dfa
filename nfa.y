%{
#include "nfa.hxx"
#include <iostream>

#define YYPARSE_PARAM ptr 
#define YYERROR_VERBOSE 1
#define YYDEBUG 1

extern int yylex (void);
void yyerror (char *s);

%}

%union {
std::string* str;
SetOfStatesT* sos;
LetterAndSetOfStatesPairT* lasosp;
StateAndDeltaPairT* sadp;
StateDeltaT* sd;
}

%token TOK_STATE, TOK_ID, TOK_ARROW, TOK_NAME, TOK_START, TOK_FINAL

%type <sadp> state
%type <sd> delta_list delta_list_final
%type <lasosp> delta
%type <sos> state_list
%type <str> TOK_ID

%start start

%%

start: automaton
;

automaton : 
  name states
;

name :
  TOK_NAME TOK_ID		{ ((NFA*)YYPARSE_PARAM)->name = *$2; }
;

states : 
  state states			{ ((NFA*)YYPARSE_PARAM)->delta.insert(*$1); }
| state				{ ((NFA*)YYPARSE_PARAM)->delta.insert(*$1); }
;

state : 
  TOK_START TOK_FINAL TOK_STATE TOK_ID '{' delta_list '}'
				{ $$ = new StateAndDeltaPairT(*$4,*$6);
				  ((NFA*)YYPARSE_PARAM)->initial = *$4; 
				  ((NFA*)YYPARSE_PARAM)->final.insert(*$4); }
| TOK_START TOK_STATE TOK_ID '{' delta_list '}' 
				{ $$ = new StateAndDeltaPairT(*$3,*$5);
				  ((NFA*)YYPARSE_PARAM)->initial = *$3; }
| TOK_FINAL TOK_STATE TOK_ID '{' delta_list_final '}' 
				{ $$ = new StateAndDeltaPairT(*$3,*$5);
				  ((NFA*)YYPARSE_PARAM)->final.insert(*$3); }
| TOK_STATE TOK_ID '{' delta_list '}'
				{ $$ = new StateAndDeltaPairT(*$2,*$4); }
;

delta_list_final : 
  delta_list			{ $$ = $1; }
| /* empty */			{ $$ = new StateDeltaT; }
;

delta_list : 
  delta ',' delta_list		{ $3->insert(*$1); $$ = $3; }
| delta delta_list		{ $2->insert(*$1); $$ = $2; }
| delta				{ $$ = new StateDeltaT; $$->insert(*$1); }
;

delta : 
  TOK_ID TOK_ARROW '{' state_list '}'
				{ $$ = new LetterAndSetOfStatesPairT(*$1,*$4); }
| TOK_ID TOK_ARROW TOK_ID	{ SetOfStatesT sos;
				  sos.insert(*$3);
				  $$ = new LetterAndSetOfStatesPairT(*$1,sos); }
;

state_list :
  TOK_ID ',' state_list		{ $3->insert(*$1); $$ = $3; }  
| TOK_ID state_list		{ $2->insert(*$1); $$ = $2; } 
| TOK_ID			{ $$ = new SetOfStatesT; $$->insert(*$1); }
;

%%

void yyerror (char *s)
{		
    std::cerr << s << std::endl;
}
