#include <iostream>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <utility>
#include <deque>
#include <string>
#include <stdlib.h>
#include "nfa.hxx"

extern int yyparse (void*);
extern FILE* yyin;
extern std::deque<std::string > qq;

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
 
    NFA nfa;
    int ret = yyparse((void*)&nfa);
    std::cerr << "yyparse() == " << ret << std::endl;
    if (ret == 1) {
	std::cerr << "chyba pri parsovani vstupu" << std::endl;
	std::copy(qq.begin(),qq.end(),
		  std::ostream_iterator<std::string >(std::cout," => "));
	exit(EXIT_FAILURE);
    }

    for (DeltaMappingT::const_iterator i = nfa.delta.begin();
	 i != nfa.delta.end();
	 ++i) {
	std::cout << i->first << std::endl;
    }

}
