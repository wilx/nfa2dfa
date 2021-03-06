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
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <popt.h>
#include "nfa.hxx"


extern int yyparse (void*);
extern FILE* yyin;
extern char* filename;

#define OPT_RENAME 0
#define OPT_DOT 1
#define OPT_VCG 2
#define OPT_NOTHING 3
#define OPT_INFILE 4
#define OPT_OUTFILE 5
#define OPT_MINIMIZE 6
#define OPT_COUNT (OPT_MINIMIZE+1)

union optionValue
{
  char* str;
  int val;
};

optionValue options[OPT_COUNT] = {{0}};
const poptOption optionsDesc[] = {
  {"rename", 'r', POPT_ARG_NONE, &options[OPT_RENAME], 1,
   "rename states", NULL},
  {"dot", 'd', POPT_ARG_NONE, &options[OPT_DOT], 1,
   "generate dot graph description", NULL},
  {"vcg", 'v', POPT_ARG_NONE, &options[OPT_VCG], 1,
   "generate VCG graph description", NULL},
  {"do-nothing", 'n', POPT_ARG_NONE, &options[OPT_NOTHING], 1,
   "don't convert, just print input to output", NULL},
  {"input", 'i', POPT_ARG_STRING, &options[OPT_INFILE], 1,
   "name of input file","file name"},
  {"output", 'o', POPT_ARG_STRING, &options[OPT_OUTFILE], 1,
   "name of output file", "file name"},
  {"minimize", 'm', POPT_ARG_NONE, &options[OPT_MINIMIZE], 1,
   "minimize states", NULL},
  POPT_AUTOHELP
  {NULL, '\0', 0, 0, 0, NULL, NULL}
};

int
main (int argc, const char* argv[])
{
  try
    {
      /* Analyzuj prikazovou radku pro paramatry. */
      poptContext context = poptGetContext(NULL, argc, argv, optionsDesc,0);
      int rc;
      while ((rc = poptGetNextOpt(context)) > 0);
      if (rc < -1)
        {
          std::cerr << poptStrerror(rc) << std::endl;
          poptPrintHelp(context, stderr, 0);
          exit(EXIT_FAILURE);
        }

      /* Osetreni parametru vstupniho souboru */
      if (options[OPT_INFILE].str != NULL)
        {
          if (!(yyin = fopen(options[OPT_INFILE].str,"r")))
            {
              perror("chyba fopen()");
              exit(EXIT_FAILURE);
            }
          filename = options[OPT_INFILE].str;
        }
      else
        {
          yyin = stdin;
          filename = "<stdin>";
        }

      /* Osetreni parametru vystupniho souboru */
      std::ofstream outfile;
      std::ostream* os = NULL;
      if (options[OPT_OUTFILE].str != NULL)
        {
          outfile.open(options[OPT_OUTFILE].str);
          if (!outfile)
            {
              std::cerr << "chyba pri otevirani vystupniho souboru"
                        << std::endl;
              exit(EXIT_FAILURE);
            }
          os = &outfile;
        }
      else
        {
          os = &std::cout;
        }

      NFA nfa;
      NFA_conv nfa_conv;
      int ret = yyparse((void*)&nfa);
      if (ret == 1)
        {
          std::cerr << "chyba pri parsovani vstupu" << std::endl;
          exit(EXIT_FAILURE);
        }

      if (! options[OPT_NOTHING].val)
        {
          remove_epsilons (nfa);
          nfa_conv = convert_NFA2DFA(nfa);
          nfa = fix_converted(nfa_conv);
        }

      if (options[OPT_MINIMIZE].val)
        minimize (nfa);

      if (options[OPT_RENAME].val)
        rename_states(nfa);

      if (options[OPT_DOT].val)
        *os << printNFA2dot(nfa);
      else if (options[OPT_VCG].val)
        *os << printNFA2vcg(nfa);
      else
        *os << printNFA(nfa);
    }
  catch (const state_not_found& e)
    {
      std::cerr << "chyba pri prevodu v convert_NFA2DFA(): "
                << e.what() << std::endl;
      std::cerr << "zobrazeni delta na neznamy stav" << std::endl;
      exit(EXIT_FAILURE);
    }
  catch (const std::exception& e)
    {
      std::cerr << "vyjimka: " << e.what() << std::endl;
      exit(EXIT_FAILURE);
    }
  catch (const std::string& e)
    {
      std::cerr << "vyjimka: " << e << std::endl;
      exit(EXIT_FAILURE);
    }
  catch (...)
    {
      std::cerr << "nastala neznama vyjimka" << std::endl;
      exit(EXIT_FAILURE);
    }

  exit(EXIT_SUCCESS);
}
