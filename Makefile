LIBS = -lfl
CXX = c++
CXXFLAGS = -ggdb
EXE = .exe

all : test

test : testgram.o lexer.o parser.o
	$(CXX) -o $@ $^ $(LIBS)

testgram.o : testgram.cxx

testgram.cxx : nfa.hxx

lexer.o : lexer.cxx

lexer.cxx : nfa.lex nfa.hxx parser.hxx
	flex -8 -b -c -d -o$@ $<

#nfa.lex : parser.hxx

parser.o : parser.cxx

parser.cxx : nfa.y nfa.hxx parser.hxx
	bison -k -v -d -g -t -o$@ $^

parser.hxx : nfa.y nfa.hxx
	bison -k -v -d -g -t -o$@ $^

%.o : %.cxx
	$(CXX) $(CXXFLAGS) $< -c -o $@

.PHONY : all test
