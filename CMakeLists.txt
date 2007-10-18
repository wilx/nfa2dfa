PROJECT (nfa2dfa)

SET(CMAKE_VERBOSE_MAKEFILE on)

FIND_PATH(
  POPT_INCLUDE_DIR
  NAMES popt.h
  PATHS $ENV{POPT_INCLUDE_DIR} /usr/include /usr/local/include
)

FIND_LIBRARY(
  POPT_LIBRARY 
  NAMES popt libpopt 
  PATHS $ENV{POPT_LIBRARY_DIR} /usr/lib /usr/local/lib
)

FIND_LIBRARY(
  FL_LIBRARY 
  NAMES fl libfl
  PATHS $ENV{FL_LIBRARY_DIR} /usr/lib /usr/local/lib
)

ADD_CUSTOM_COMMAND(
    OUTPUT lexer.cxx lexer.hxx
    COMMAND flex
    ARGS -olexer.cxx nfa.lex
    DEPENDS nfa.lex
    WORKING_DIRECTORY ${nfa2dfa_SOURCE_DIR}
    COMMENT "Generating lexer using flex."
)

ADD_CUSTOM_COMMAND(
    OUTPUT parser.cxx parser.hxx
    COMMAND bison
    ARGS -y -d -oparser.cxx nfa.y
    DEPENDS nfa.y
    WORKING_DIRECTORY ${nfa2dfa_SOURCE_DIR}
    COMMENT "Generating parser using bison."
)

INCLUDE_DIRECTORIES(${POPT_INCLUDE_DIR})
ADD_EXECUTABLE(nfa2dfa nfa2dfa.cxx nfa.cxx lexer.cxx parser.cxx)

TARGET_LINK_LIBRARIES(nfa2dfa ${POPT_LIBRARY} ${FL_LIBRARY})
