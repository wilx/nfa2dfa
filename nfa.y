%{
#include "nfa.hxx"
#include <iostream>

#define YYPARSE_PARAM ptr 
#define YYERROR_VERBOSE 1
#define YYDEBUG 1

#define DBG(x) { std::cerr << x << std::endl; }

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

%token TOK_STATE, TOK_ID, TOK_ARROW

%type <sadp> state
%type <sd> delta_list
%type <lasosp> delta
%type <sos> state_list
%type <str> TOK_ID

%%

start : 
  state start                             	{ ((NFA*)YYPARSE_PARAM)->insert(*$1); }
| state                                         { ((NFA*)YYPARSE_PARAM)->insert(*$1); }
| /* empty */
;

state : TOK_STATE TOK_ID '{' delta_list '}'	{ $$ = new StateAndDeltaPairT(*$2,*$4); }
;

delta_list : 
  delta ',' delta_list				{ $3->insert(*$1); $$ = $3; }
| delta delta_list                   		{ $2->insert(*$1); $$ = $2; }
| delta                                         { $$ = new StateDeltaT; $$->insert(*$1); }
| /* empty */                                   { $$ = new StateDeltaT; }
;

delta : 
  TOK_ID TOK_ARROW '{' state_list '}'		{ $$ = new LetterAndSetOfStatesPairT(*$1,*$4); }
| TOK_ID TOK_ARROW TOK_ID			{ SetOfStatesT* sos = new SetOfStatesT;
						  sos->insert(*$3);
						  $$ = new LetterAndSetOfStatesPairT(*$1,*sos); }
;

state_list :
  TOK_ID ',' state_list				{ $3->insert(*$1); $$ = $3; }  
| TOK_ID state_list                             { $2->insert(*$1); $$ = $2; } 
| TOK_ID                                        { $$ = new SetOfStatesT; $$->insert(*$1); }
| /* empty */                                   { $$ = new SetOfStatesT; }
;

%%

void yyerror (char *s)
{		
    fprintf (stderr, "%s\n", s);
}
