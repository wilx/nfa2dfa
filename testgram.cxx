#include <iostream>
#include <cstdio>
#include "nfa.hxx"

extern int yyparse (void);

NFA* nfa;

int main(void)
{
    nfa = new NFA;
    return yyparse();
}
