%{
#include <string>
#include <iostream>
#include "yynfa.hxx"
#include "parser.hxx"
%}

%pointer

%%

[[:space:]\n]
#.*$			/* ignore comments */
"{"			{ return '{'; }
"}"			{ return '}'; }
","                     { return ','; }
->			{ return TOK_ARROW; }
state			{ return TOK_STATE; }
[[:alnum:]]+		{ yylval.str = new std::string(yytext); return TOK_ID; }
.			{ }

%%
