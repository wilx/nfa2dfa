#ifndef NFA_HXX
#define NFA_HXX

#include <string>
#include <set>
#include <map>
#include <utility>

typedef std::string StateT;
typedef std::string LetterT;
typedef std::set<StateT > SetOfStatesT;
typedef std::map<LetterT, SetOfStatesT > StateDeltaT;
typedef std::map<StateT, StateDeltaT > NFA;

/*
  Pomocne typy urcene prevazne pro pouziti v parseru.
 */
typedef std::pair<StateT, StateDeltaT > StateAndDeltaPairT;
typedef std::pair<LetterT, SetOfStatesT > LetterAndSetOfStatesPairT;

#endif
