#ifndef NFA_HXX
#define NFA_HXX

#include <string>
#include <set>
#include <map>
#include <utility>
#include <sstream>
#include <iterator>

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

/* Pomocne typy urcene prevazne pro pouziti v parseru. */
typedef std::pair<StateT, StateDeltaT > StateAndDeltaPairT;
typedef std::pair<LetterT, SetOfStatesT > LetterAndSetOfStatesPairT;


/* Funkce pro vystup do streamu */
static std::string printNFA (const NFA& nfa)
{
    std::ostringstream ss;
    
    ss << "name " << nfa.name << std::endl;
    for (DeltaMappingT::const_iterator dmi = nfa.delta.begin();
	 dmi != nfa.delta.end();
	 ++dmi) {
	const StateT& st = dmi->first;
	if (st == nfa.initial)
	    ss << "initial ";
	if (nfa.final.find(st) != nfa.final.end())
	    ss << "final ";
	ss << "state " << st << " {" << std::endl;
	for (StateDeltaT::const_iterator sdi = dmi->second.begin();
	     sdi != dmi->second.end();
	     ++sdi) {
	    ss << sdi->first << " -> { ";
	    std::copy(sdi->second.begin(), sdi->second.end(),
		      std::ostream_iterator<StateT >(ss," "));
	    ss << "}" << std::endl;
	}
	ss << "}" << std::endl;
    }

    return ss.str();
}

#endif
