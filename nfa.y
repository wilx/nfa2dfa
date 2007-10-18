%{
/*
Copyright (c) 2002-2007, Václav Haisman

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "nfa.hxx"
#include <iostream>

#define YYPARSE_PARAM ptr 
#define YYERROR_VERBOSE 1
#define YYDEBUG 1

char* filename = "";
extern int yylineno;
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

%token TOK_STATE TOK_ID TOK_ARROW TOK_NAME TOK_START TOK_FINAL

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
    std::cerr << filename << ":" << yylineno << " " << s << std::endl;
}
