BISON = bison
FLEX = flex
LIBS = -lfl
CXX = c++
#CXXFLAGS = -ggdb -W -Wall -Wno-unused -pedantic -O2 -march=athlon-tbird \
#	-save-temps -fverbose-asm -fprefetch-loop-arrays -funroll-loops \
#	-fomit-frame-pointer
CXXFLAGS = -g3 -W -Wall -Wno-unused -pedantic
EXE = .exe

all : test nfa2dfa

nfa2dfa : nfa2dfa.o lexer.o parser.o nfa.o
	$(CXX) -o $@ $^ $(LIBS)

nfa2dfa.o : nfa2dfa.cc nfa.hxx

nfa.o : nfa.cxx nfa.hxx

test : testgram.o lexer.o parser.o nfa.o
	$(CXX) -o $@ $^ $(LIBS)

testgram.o : testgram.cxx nfa.hxx

lexer.o : lexer.cxx nfa.hxx parser.hxx

lexer.cxx : nfa.lex
	$(FLEX) -8 -o$@ $<

parser.o : parser.cxx parser.hxx nfa.hxx

parser.cxx : nfa.y
	$(BISON) -k -d -o$@ $<

parser.hxx : parser.cxx

%.o : %.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean : 
	rm -rf *.o
	rm -rf *.exe
	rm -rf parser.[ch]xx
	rm -rf lexer.cxx

.PHONY : all test clean
