#include <iostream>
#include <cstdio>
#include "nfa.hxx"

extern int yyparse (void*);
extern FILE* yyin;

int main(void)
{
    NFA* nfa = new NFA;
    return yyparse((void*)nfa);
    delete nfa;
}
