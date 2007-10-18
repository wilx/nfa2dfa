/*
Copyright (c) 2002-2007, Václav Haisman

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
