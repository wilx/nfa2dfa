#ifndef BISON_PARSER_HXX
# define BISON_PARSER_HXX

#ifndef YYSTYPE
typedef union {
std::string* str;
SetOfStatesT* sos;
LetterAndSetOfStatesPairT* lasosp;
StateAndDeltaPairT* sadp;
StateDeltaT* sd;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	TOK_STATE	257
# define	TOK_ID	258
# define	TOK_ARROW	259
# define	TOK_NAME	260
# define	TOK_START	261
# define	TOK_FINAL	262


extern YYSTYPE yylval;

#endif /* not BISON_PARSER_HXX */
