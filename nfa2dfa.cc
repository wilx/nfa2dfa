#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <utility>
#include <deque>
#include <string>
#include <cstdlib>
#include <exception>
#include <popt.h>
#include "nfa.hxx"

extern int yyparse (void*);
extern FILE* yyin;
extern char* filename;

#define OPT_CONVERT 0
#define OPT_RENAME 1
#define OPT_DOT 2
#define OPT_VCG 3
#define OPT_NOTHING 4
#define OPT_INFILE 5
#define OPT_OUTFILE 6
#define OPT_COUNT OPT_OUTFILE+1

union optionValue {
    char* str;
    int val;
};

optionValue options[OPT_COUNT];
const poptOption optionsDesc[] = {
    {"convert",'c',POPT_ARG_NONE,&options[OPT_CONVERT],1,
     "convert NFA to DFA",NULL},
    {"rename",'r',POPT_ARG_NONE,&options[OPT_RENAME],1,
     "rename states",NULL},
    {"dot",'d',POPT_ARG_NONE,&options[OPT_DOT],1,
     "generate dot graph description",NULL},
    {"vcg",'v',POPT_ARG_NONE,&options[OPT_VCG],1,
     "generate VCG graph description",NULL},
    {"do-nothing",'n',POPT_ARG_NONE,&options[OPT_NOTHING],1,
     "do nothing, just print input to output",NULL},
    {"input",'i',POPT_ARG_STRING,&options[OPT_INFILE],1,
     "name of input file","file name"},
    {"output",'o',POPT_ARG_STRING,&options[OPT_OUTFILE],1,
     "name of output file","file name"},
    POPT_AUTOHELP
    {NULL, '\0', 0, 0, 0, NULL, NULL}
};

int main(int argc, const char* argv[])
{
    try {
	/* Analyzuj prikazovou radku pro paramatry. */
	poptContext context = poptGetContext(NULL, argc, argv, optionsDesc,0);
	int rc;
	while ((rc = poptGetNextOpt(context)) > 0);
	if (rc < -1) {
	    std::cerr << poptStrerror(rc) << std::endl;
	    poptPrintHelp(context, stderr, 0);
	    exit(EXIT_FAILURE);
	}
	
	/* Osetreni parametru vstupniho souboru */
	if (options[OPT_INFILE].str != NULL) {
	    if (!(yyin = fopen(options[OPT_INFILE].str,"r"))) {
		perror("chyba fopen()");
		exit(EXIT_FAILURE);
	    }
	    filename = options[OPT_INFILE].str;
	}
	else {
	    yyin = stdin;
	    filename = "<stdin>";
	}
	
	/* Osetreni parametru vystupniho souboru */
	std::ofstream outfile;
	std::ostream* os = NULL;
	if (options[OPT_OUTFILE].str != NULL) {
	    outfile.open(options[OPT_OUTFILE].str);
	    if (!outfile) {
		std::cerr << "chyba pri otevirani vystupniho souboru" 
			  << std::endl;
		exit(EXIT_FAILURE);
	    }
	    os = &outfile;
	}
	else {
	    os = &std::cout;
	}
 
	NFA nfa;
	NFA_conv nfa_conv;
	int ret = yyparse((void*)&nfa);
	if (ret == 1) {
	    std::cerr << "chyba pri parsovani vstupu" << std::endl;
	    exit(EXIT_FAILURE);
	}

	if (options[OPT_NOTHING].val) {
	    *os << printNFA(nfa);
	    exit(EXIT_SUCCESS);
	}
	
	if (options[OPT_CONVERT].val) {
	    nfa_conv = convert_NFA2DFA(nfa);
	    nfa = fix_converted(nfa_conv);
	}
       
	if (options[OPT_RENAME].val) {
	    rename_states(nfa);
	}
	
	if (options[OPT_DOT].val)
	    *os << printNFA2dot(nfa);
	else
	    if (options[OPT_VCG].val)
		*os << printNFA2vcg(nfa);
	    else
		*os << printNFA(nfa);
    }
    catch (state_not_found& e) {
	std::cerr << "chyba pri prevodu v convert_NFA2DFA(): " 
		  << e.what() << std::endl;
	std::cerr << "zobrazeni delta na neznamy stav" << std::endl;
	exit(EXIT_FAILURE);
    }
    catch (std::exception& e) {
	std::cerr << "vyjimka: " << e.what() << std::endl;
	exit(EXIT_FAILURE);
    }
    catch (...) {
	std::cerr << "nastala neznama vyjimka" << std::endl;
	exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
