#include "nfa.hxx"
#include <iterator>

const char* state_not_found::what () const throw ()
{
    return "state not found in delta mapping";
}

std::string printNFA (const NFA& nfa)
{
    std::ostringstream ss;
    
    ss << "automaton " << nfa.name << std::endl;
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
	    ss << sdi->first << " -> ";
	    if (sdi->second.size() > 1)
		ss << "{ ";
	    std::copy(sdi->second.begin(), sdi->second.end(),
		      std::ostream_iterator<StateT >(ss," "));
	    if (sdi->second.size() > 1)
		ss << "}";
	    ss << std::endl;
	}
	ss << "}" << std::endl;
    }

    return ss.str();
}

std::string printNFA (const NFA_conv& nfa)
{
    std::ostringstream ss;
    
    ss << "automaton " << nfa.name << std::endl;
    for (std::map<SetOfStatesT, StateDeltaT >::const_iterator dmi 
	     = nfa.delta.begin();
	 dmi != nfa.delta.end();
	 ++dmi) {
	const SetOfStatesT& st = dmi->first;
	if (st == nfa.initial)
	    ss << "initial ";
	if (nfa.final.find(st) != nfa.final.end())
	    ss << "final ";
	ss << "state " << join_seq(std::string("-"),st) 
	   << " {" << std::endl;
	for (StateDeltaT::const_iterator sdi = dmi->second.begin();
	     sdi != dmi->second.end();
	     ++sdi) {
	    ss << sdi->first << " -> " 
	       << join_seq(std::string("-"),sdi->second)
	       << std::endl;
	}
	ss << "}" << std::endl;
    }
    
    return ss.str();
}

/* Get set of input alphabet T from automaton's delta mapping. */
std::set<LetterT > input_alphabet (const NFA& nfa)
{
    std::set<LetterT> alphabet;
    for (DeltaMappingT::const_iterator i = nfa.delta.begin();
	 i != nfa.delta.end();
	 ++i) {
	for (StateDeltaT::const_iterator k = i->second.begin();
	     k != i->second.end();
	     ++k) {
	    alphabet.insert(k->first);
	}
    }
    return alphabet;
}

NFA_conv convert_NFA2DFA (const NFA& nfa)
{
    NFA_conv nfa_conv;
    /* 1. The set Q' = {{q0}} will be defined, the state {q0} will be treated
       as unmarked. */
    std::set<LetterT > alphabet = input_alphabet(nfa);
    std::set<SetOfStatesT > Qnew, unmarked, marked;
    SetOfStatesT q0set;
    q0set.insert(nfa.initial);
    Qnew.insert(q0set);
    unmarked.insert(q0set);
    
    /* 2. If each state in Q' is marked, then continue with step (4). */
    while (! unmarked.empty()) {
	/* 3. An unmarked state q' will be chosen from Q' and th following
	   operations will be executed: */
	SetOfStatesT q_ = *unmarked.begin();
	/* (a) We will determine delta'(q',a) = union(delta(p,a)) for
	   p element q' and for all a element T. */
	StateDeltaT stdelta;
	for (std::set<LetterT >::const_iterator letter = alphabet.begin();
	     letter != alphabet.end();
	     ++letter) {
	    /* alokace promenne pro sjednoceni union(delta(p,a)) 
	       for p element q' */
	    SetOfStatesT* un = new SetOfStatesT;
	    for (SetOfStatesT::const_iterator p = q_.begin();
		 p != q_.end();
		 ++p) {
		// najdi StateDeltaT pro stav
		DeltaMappingT::const_iterator dmi = nfa.delta.find(*p);
		// element nenalezen
		if (dmi == nfa.delta.end())
		    throw state_not_found();
		const StateDeltaT& sd = dmi->second;
		/* Najdi mnozinu cilovych stavu ze stavu *p pri 
		   vstupnim pismenu *letter. */
		StateDeltaT::const_iterator sdi = sd.find(*letter);
		/* Element nenalezen, tzn. z tohodle stavu pri
		   vstupu *letter nevede zadna cesta. */
		if (sdi == sd.end())
		    continue;
		un->insert(sdi->second.begin(), sdi->second.end());
	    }
	    /* Nevkladej prazdne stavy. */
	    if (un->empty())
		continue;
	    stdelta.insert(make_pair(*letter,*un));
	    /* (b) Q' = Q' union delta'(q',a) for all a element T. */
	    Qnew.insert(*un);
	    /* pokud stav jeste neni oznaceny */
	    if (marked.find(*un) == marked.end())
		/* pridame ho k neoznacenym */
		unmarked.insert(*un);
	    
	}
	/* (c) The state q' element Q' will be marked. */
	unmarked.erase(q_);
	marked.insert(q_);
	/* (d) Continue with step (2). */
	nfa_conv.delta.insert(make_pair(q_,stdelta));
    }
    
    /* 4. q0' = {q0}. */
    nfa_conv.initial = q0set;
    /* 5. F' = {q' : q' element Q', q' intersection F != 0}. */
    for (std::set<SetOfStatesT >::const_iterator q_ = Qnew.begin();
	 q_ != Qnew.end();
	 ++q_) {
	// promenna pro prunik
	SetOfStatesT inter;
	set_intersection(q_->begin(), q_->end(),
			 nfa.final.begin(), nfa.final.end(),
			 std::insert_iterator<SetOfStatesT >(inter,
							     inter.begin()));
	if (! inter.empty())
	    nfa_conv.final.insert(*q_);
    }
    nfa_conv.name = nfa.name;
    
    return nfa_conv;
}

static void next_name (std::string& s)
{
    if (s[s.length()-1] == 'Z') {
	s += 'A';
    }
    else {
	s[s.length()-1] += 1;
    }
}

NFA fix_converted (const NFA_conv& nfa_conv, bool rename)
{
    NFA nfa;
    std::string stname("A");
    std::map<SetOfStatesT, StateT > table;
    std::map<SetOfStatesT, StateDeltaT >::const_iterator dmi;

    /* Vytvor prevodni tabulku jmen stavu. */
    for (dmi = nfa_conv.delta.begin();
	 dmi != nfa_conv.delta.end();
	 ++dmi) {
	if (rename) {
	    table.insert(make_pair(dmi->first, stname));
	    next_name(stname); 
	}
	else 
	    table.insert(make_pair(dmi->first, 
				   join_seq(std::string("-"),
					    dmi->first)));
    }
    
    for (dmi = nfa_conv.delta.begin();
	 dmi != nfa_conv.delta.end();
	 ++dmi) {
	StateDeltaT sd;
	for (StateDeltaT::const_iterator sdi = dmi->second.begin();
	     sdi != dmi->second.end();
	     ++sdi) {
	    SetOfStatesT s;
	    s.insert(table.find(sdi->second)->second);
	    sd.insert(make_pair(sdi->first,s));
	}
	nfa.delta.insert(make_pair(table.find(dmi->first)->second,sd));
    }
    
    nfa.name = nfa_conv.name;
    nfa.initial = table.find(nfa_conv.initial)->second;
    for (std::set<SetOfStatesT >::const_iterator si = nfa_conv.final.begin();
	 si != nfa_conv.final.end();
	 ++si)
	nfa.final.insert(table.find(*si)->second);
    
    return nfa;
}

