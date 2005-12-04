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
typedef std::set<StateT> SetOfStatesT;
typedef std::map<LetterT, SetOfStatesT> StateDeltaT;
typedef std::map<StateT, StateDeltaT> DeltaMappingT;
struct NFA
{
    std::string name;
    StateT initial;
    SetOfStatesT final;
    DeltaMappingT delta;
};

struct NFA_conv
{
    std::string name;
    SetOfStatesT initial;
    std::set<SetOfStatesT> final;
    std::map<SetOfStatesT, StateDeltaT> delta;
};

/* Pomocne typy urcene prevazne pro pouziti v parseru. */
typedef std::pair<StateT, StateDeltaT> StateAndDeltaPairT;
typedef std::pair<LetterT, SetOfStatesT> LetterAndSetOfStatesPairT;

/* Vyjimky */
class state_not_found : public std::exception
{
public:
    virtual const char* what () const throw ();
};

/* Funkce pro vystup do streamu */
template <typename Res, typename Seq>
inline
Res
join_seq (const Res& sep, const Seq& seq)
{
    if (seq.empty())
	return Res();

    std::basic_ostringstream<typename Res::value_type > ss;
    typename Seq::const_iterator it = seq.begin();
    ss << *it;
    ++it;
    for (; it != seq.end (); ++it)
      ss << sep << *it;

    return ss.str();
}

extern std::string printNFA (const NFA& nfa);
extern std::string printNFA (const NFA_conv& nfa);
extern std::string printNFA2dot (const NFA& nfa);
extern std::string printNFA2vcg (const NFA& nfa);
extern std::set<LetterT > input_alphabet (const NFA& nfa);
extern SetOfStatesT automaton_states (const NFA& nfa);
extern NFA_conv convert_NFA2DFA (const NFA& nfa);
extern NFA fix_converted (const NFA_conv& nfa_conv, const bool rename = false);
extern NFA& rename_states(NFA& nfa);
extern void remove_epsilons (NFA &);
extern NFA remove_epsilons (NFA const &);
extern void minimize (NFA& nfa);

#endif
