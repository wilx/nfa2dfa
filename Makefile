LIBS = -lfl
CXX = c++
# CXXFLAGS = -ggdb -W -Wall -Wno-unused -pedantic -O2 -march=athlon-tbird \
#	-save-temps -fverbose-asm -fprefetch-loop-arrays -funroll-loops
CXXFLAGS = -ggdb -W -Wall -Wno-unused -pedantic
EXE = .exe

all : test

test : testgram.o lexer.o parser.o
	$(CXX) -o $@ $^ $(LIBS)

testgram.o : testgram.cxx nfa.hxx

lexer.o : lexer.cxx nfa.hxx parser.hxx

lexer.cxx : nfa.lex
	flex -d -B -8 -o$@ $<

parser.o : parser.cxx nfa.hxx

parser.cxx : nfa.y
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
