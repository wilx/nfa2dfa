#ifndef MISSING_HXX
#define MISSING_HXX

#include "config.h"

#if defined(HAVE_STD_SELETCT1ST)
using std::select1st;
#elif defined(HAVE_GNU_EXT_SELECT1ST)
# include <ext/functional>
using __gnu_cxx::select1st;
#else
/* Copied from SGI's STL */
// select1st and select2nd are extensions: they are not part of the standard.
template <class _Pair>
struct select1st 
    : public std::unary_function<_Pair, typename _Pair::first_type> {
  const typename _Pair::first_type& operator()(const _Pair& __x) const {
    return __x.first;
  }
};
#endif

#endif // MISSING_HXX
