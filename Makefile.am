bin_PROGRAMS = nfa2dfa
nfa2dfa_SOURCES = nfa2dfa.cc nfa.cxx nfa.hxx parser.cxx parser.hxx \
	lexer.cxx
nfa2dfa_LDADD = @LEXLIB@
EXTRA_DIST = tests/automaton*.txt tests/empty.txt

lexer.cxx : nfa.lex
	@LEX@ -o$@ $<

parser.cxx : nfa.y
	@YACC@ -d -o$@ $<

parser.hxx : parser.cxx
