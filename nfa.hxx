#ifndef NFA_HXX
#define NFA_HXX

#include <string>
#include <set>
#include <map>
#include <utility>
#include <sstream>
#include <exception>
#include "config.h"

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

/* Vyjimky */
class state_not_found : public std::exception {
public:
    virtual const char* what () const throw ();
};

/* Funkce pro vystup do streamu */
template <typename Res, typename Seq>
Res join_seq (const Res& sep, const Seq& seq)
{
    if (seq.empty())
	return Res();

    std::basic_ostringstream<typename Res::value_type > ss;
    typename Seq::const_iterator it = seq.begin();
    typename Seq::const_iterator next = it;
    ++next;
    for (; 
	 next != seq.end();
	 ++it, ++next) {
	ss << *it << sep;
    }
    ss << *it;

    return ss.str();
}

std::string printNFA (const NFA& nfa);
std::string printNFA (const NFA_conv& nfa);
std::string printNFA2dot (const NFA& nfa);
std::set<LetterT > input_alphabet (const NFA& nfa);
NFA_conv convert_NFA2DFA (const NFA& nfa);
NFA fix_converted (const NFA_conv& nfa_conv, bool rename = false);
void simplify (NFA& nfa);

#endif
