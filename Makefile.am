AM_CXXFLAGS = @CXXFLAGS@ -W -Wall -pedantic
bin_PROGRAMS = nfa2dfa
nfa2dfa_SOURCES = nfa2dfa.cxx nfa.cxx nfa.hxx parser.cxx parser.hxx
nfa2dfa_SOURCES += lexer.cxx missing.hxx
nfa2dfa_LDADD = @LEXLIB@
EXTRA_DIST = nfa.lex nfa.y tests/automaton*.txt tests/empty.txt
EXTRA_DIST += COPYING reconf config.sub config.guess README

lexer.cxx : nfa.lex
	@LEX@ -o$@ $<

parser.cxx : nfa.y
	@YACC@ -d -o$@ $<

parser.hxx : parser.cxx
