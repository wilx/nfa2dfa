#ifndef NFA_HXX
#define NFA_HXX

#include <string>
#include <set>
#include <map>
#include <utility>
#include <iostream>

typedef std::string StateT;
typedef std::string LetterT;
typedef std::set<StateT > SetOfStatesT;
typedef std::map<LetterT, SetOfStatesT > StateDeltaT;
typedef std::map<StateT, StateDeltaT > DeltaMappingT;
struct NFA {
    std::string name;
    StateT initial;
    SetOfStatesT final;
    DeltaMappingT delta;
};

struct NFA_conv {
    std::string name;
    SetOfStatesT initial;
    std::set<SetOfStatesT > final;
    std::map<SetOfStatesT, StateDeltaT > delta;
};

/*
  Pomocne typy urcene prevazne pro pouziti v parseru.
 */
typedef std::pair<StateT, StateDeltaT > StateAndDeltaPairT;
typedef std::pair<LetterT, SetOfStatesT > LetterAndSetOfStatesPairT;



#endif
