LIBS = -lfl
CXX = c++
CXXFLAGS = -ggdb
EXE = .exe

all : test

test : testgram.o lexer.o parser.o
	$(CXX) -o $@ $^ $(LIBS)

testgram.o : testgram.cxx

lexer.o : lexer.cxx

lexer.cxx : nfa.lex
	flex -8 -b -c -d -o$@ $^

nfa.lex : parser.hxx

parser.o : parser.cxx

parser.cxx : nfa.y parser.hxx
	bison -k -v -d -g -t -d -o$@ $<

parser.hxx : yynfa.hxx

%.o : %.cxx
	$(CXX) $(CXXFLAGS) $< -c -o $@

.PHONY : all test
