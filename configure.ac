dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.59])
AC_INIT([nfa2dfa.cxx])
AM_INIT_AUTOMAKE([nfa2dfa], [0.2])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AC_LANG([C++])
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_LEX
AC_PROG_YACC
AC_CHECK_HEADER(popt.h,,[AC_MSG_ERROR([popt.h not found])])
AC_CHECK_LIB(popt,poptGetContext,,[AC_MSG_ERROR([libpopt not found])])
AX_SELECT1ST

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
