#include <iostream>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <utility>
#include <deque>
#include <string>
#include <stdlib.h>
#include "nfa.hxx"

extern int yyparse (void*);
extern FILE* yyin;
extern std::deque<std::string > qq;

int main(int argc, char* argv[])
{
    if (argc == 2) {
	if (!(yyin = fopen(argv[1],"r"))) {
	    perror("chyba fopen()");
	    exit(EXIT_FAILURE);
	}
    }
    else
	yyin = stdin;
 
    NFA nfa;
    NFA_conv nfa_conv;
    int ret = yyparse((void*)&nfa);
    if (ret == 1) {
	std::cerr << "chyba pri parsovani vstupu" << std::endl;
	std::copy(qq.begin(),qq.end(),
		  std::ostream_iterator<std::string >(std::cout," => "));
	exit(EXIT_FAILURE);
    }
    std::cout << printNFA(nfa) << std::endl;
    
    /* Get set of input alphabet T from automaton's delta mapping. */
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

    /* 1. The set Q' = {{q0}} will be defined, the state {q0} will be treated
       as unmarked. */
    std::set<SetOfStatesT > Qnew, unmarked;
    SetOfStatesT q0set;
    q0set.insert(nfa.initial);
    Qnew.insert(q0set);
    unmarked.insert(q0set);
    
    /* 2. If each state in Q' is marked, then continue with step (4). */
    while (! unmarked.empty()) {
	std::cerr << "unmarked.size()==" << unmarked.size() << std::endl;
	/* 3. An unmarked state q' will be chosen from Q' and th following
	   operations will be executed: */
	SetOfStatesT q_ = *unmarked.begin();
	/* (a) We will determine delta'(q',a) = union(delta(p,a)) for
	   p element q' and for all a element T. */
	StateDeltaT stdelta;
	int cnt = 0;
	for (std::set<LetterT >::const_iterator letter = alphabet.begin();
	     letter != alphabet.end();
	     ++letter) {
	    std::cerr << "cnt==" << ++cnt << std::endl;
	    /* alokace promenne pro sjednoceni union(delta(p,a)) 
	       for p element q' */
	    SetOfStatesT* un = new SetOfStatesT;
	    int cnt2 = 0;
	    for (SetOfStatesT::const_iterator p = q_.begin();
		 p != q_.end();
		 ++p) {
		std::cerr << "cnt2==" << ++cnt2 << std::endl;
		// najdi StateDeltaT pro stav
		DeltaMappingT::const_iterator dmi = nfa.delta.find(*p);
		// element nenalezen
		if (dmi == nfa.delta.end())
		    continue;
		const StateDeltaT& sd = dmi->second;
		/* najdi mnozinu cilovych stavu ze stavu *p pri 
		   vstupnim pismenu *letter */
		StateDeltaT::const_iterator sdi = sd.find(*letter);
		// element nenalezen
		if (sdi == sd.end())
		    continue;
		un->insert(sdi->second.begin(), sdi->second.end());
	    }
	    stdelta.insert(make_pair(*letter,*un));
	    /* (b) Q' = Q' union delta'(q',a) for all a element T. */
	    Qnew.insert(*un);
	    /* (c) The state q' element Q' will be marked. */
	    unmarked.erase(q_);
	    /* (d) Continue with step (2). */
	}
	nfa_conv.delta.insert(make_pair(q_,stdelta));
    }
    
    nfa_conv.name = nfa.name;
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
 
    
}
