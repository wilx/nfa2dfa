#include <iostream>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <utility>
#include <stdlib.h>
#include "nfa.hxx"

extern int yyparse (void*);
extern FILE* yyin;

int main(int argc, char* argv[])
{
    if (argc == 2) {
	if (!(yyin = fopen(argv[1],"r"))) {
	    perror("chyba fopen()");
	    exit(EXIT_FAILURE);
	}
    }
    else
	yyin = stdin;
 
    NFA* nfa = new NFA;
    if(!yyparse((void*)nfa)) {
	std::cerr << "chyba pri parsovani vstupu" << std::endl;
	exit(EXIT_FAILURE);
    }

    for (NFA::const_iterator i = nfa->begin();
	 i != nfa->end();
	 ++i) {
	std::cout << i->first << std::endl;
    }

    delete nfa;
}
