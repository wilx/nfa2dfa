%{
#include <string>
#include "nfa.hxx"
#include "parser.hxx"
%}

%pointer
%option yylineno

%%

[[:space:]]+            /* ignore spaces */
#.*$                    /* ignore comments */
"{"                     { return '{'; }
"}"                     { return '}'; }
","                     { return ','; }
->                      { return TOK_ARROW; }
state                   { return TOK_STATE; }
initial|start(ing)?     { return TOK_START; }
name|automaton          { return TOK_NAME; }
final|accepting         { return TOK_FINAL; }
([[:alnum:]_-]|".")+    { yylval.str = new std::string(yytext); 
                          return TOK_ID; }
''                      { yylval.str = new std::string (); 
                          return TOK_ID; }

%%
