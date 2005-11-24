dnl Test for select1st functor
AC_DEFUN([AX_SELECT1ST], [
AC_PREREQ(2.59)
AC_LANG_PUSH(C++)
AH_TEMPLATE([HAVE_STD_SELECT1ST], [Define if you have std::select1st functor.])
AH_TEMPLATE([HAVE_GNU_EXT_SELECT1ST], [Define if you have __gnu_cxx:select1st functor.])
AC_MSG_CHECKING([[for select1st function object]])
select1st=no

AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE(
    [[#include <utility>
#include <functional>
]], 
    [[std::select1st<std::pair<int, int > > s1st;]])],
  [AC_DEFINE([HAVE_STD_SELECT1ST], 1, [define if compiler has std::select1st]) select1st="std::select1st"],
  [AC_COMPILE_IFELSE(
  [AC_LANG_SOURCE(
    [[#include <utility>
#include <ext/functional>
]],
    [[__gnu_cxx::select1st<std::pair<int, int > > s1st;]])],
  [AC_DEFINE([HAVE_GNU_EXT_SELECT1ST], 1, [define if compiler has __gnu_cxx::select1st]) select1st="__gnu_cxx::select1st"])
])

AC_MSG_RESULT([$select1st])
AC_LANG_POP
])
