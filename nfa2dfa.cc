#include <iostream>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <utility>
#include <deque>
#include <string>
#include <cstdlib>
#include <exception>
#include "nfa.hxx"

extern int yyparse (void*);
extern FILE* yyin;

extern char* filename;

int main(int argc, char* argv[])
{
    try {
	/* Osetreni parametru vstupniho souboru */
	if (argc == 2) {
	    if (!(yyin = fopen(argv[1],"r"))) {
		perror("chyba fopen()");
		exit(EXIT_FAILURE);
	    }
	    filename = argv[1];
	}
	else {
	    yyin = stdin;
	    filename = "<stdin>";
	}
 
	NFA nfa;
	NFA_conv nfa_conv;
	int ret = yyparse((void*)&nfa);
	if (ret == 1) {
	    std::cerr << "chyba pri parsovani vstupu" << std::endl;
	    exit(EXIT_FAILURE);
	}
	/* Ladici vypis vstupniho automatu. */
	std::cout << "input:" << std::endl << printNFA(nfa) << std::endl;

	try { 
	    nfa_conv = convert_NFA2DFA(nfa);
	}
	catch (state_not_found& e) {
	    std::cerr << "chyba pri prevodu v convert_NFA2DFA(): " 
		      << e.what() << std::endl;
	    std::cerr << "zobrazeni delta na neznamy stav" << std::endl;
	    exit(EXIT_FAILURE);
	}

	/* Ladici vypis prevedeneho automatu */
	std::cout << "converted:" << std::endl << printNFA(nfa_conv) 
		  << std::endl;
	/* Ladici vypis prevedeneho a upraveneho automatu */
	std::cout << "converted fixed:" << std::endl 
		  << printNFA(fix_converted(nfa_conv,true)) << std::endl;
	/* simplify(nfa);
	   std::cout << "simplifed: " << printNFA(nfa) << std::endl; */
	std::cout << "dot:" <<std::endl
		  << printNFA2dot(fix_converted(nfa_conv)) << std::endl;
    }
    catch (std::exception& e) {
	std::cerr << "vyjimka: " << e.what() << std::endl;
    }
    catch (...) {
	std::cerr << "nastala neznama vyjimka" << std::endl;
    }

    exit(EXIT_SUCCESS);
}
