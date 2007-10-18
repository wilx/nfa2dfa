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
