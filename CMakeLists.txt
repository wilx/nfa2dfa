PROJECT (nfa2dfa)

SET(CMAKE_VERBOSE_MAKEFILE on)

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

ADD_EXECUTABLE(nfa2dfa nfa2dfa.cxx nfa.cxx lexer.cxx parser.cxx)

TARGET_LINK_LIBRARIES(nfa2dfa popt fl)
