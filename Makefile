LIBS = -lfl
CXX = c++
CXXFLAGS = -ggdb -W -Wall -Wno-unused -pedantic -DYYPARSE_PARAM=ptr -DYYDEBUG
EXE = .exe

all : test

test : testgram.o lexer.o parser.o
	$(CXX) -o $@ $^ $(LIBS)

testgram.o : testgram.cxx

testgram.cxx : nfa.hxx

lexer.o : lexer.cxx

lexer.cxx : nfa.lex nfa.hxx parser.hxx
	flex -8 -b -c -d -o$@ $<

parser.o : parser.cxx

parser.cxx : nfa.y nfa.hxx
	bison -k -v -d -g -t -o$@ $<

parser.hxx : parser.cxx

%.o : %.cxx
	$(CXX) $(CXXFLAGS) $< -c -o $@

clean : 
	rm -rf *.o
	rm -rf *.exe
	rm -rf parser.[ch]xx
	rm -rf lexer.cxx

.PHONY : all test clean
