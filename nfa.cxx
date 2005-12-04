#include "nfa.hxx"
#include <iterator>
#include <functional>
#include <iostream>
#include "missing.hxx"
#include "join.hxx"

const char *
state_not_found::what () const throw ()
{
  return "state not found in delta mapping";
}

std::string
printNFA (const NFA& nfa)
{
  std::ostringstream ss;

  ss << "automaton " << nfa.name << std::endl;
  for (DeltaMappingT::const_iterator dmi = nfa.delta.begin();
       dmi != nfa.delta.end();
       ++dmi)
    {
      const StateT& st = dmi->first;
      if (st == nfa.initial)
        ss << "initial ";
      if (nfa.final.find(st) != nfa.final.end())
        ss << "final ";
      ss << "state " << st << " {" << std::endl;
      for (StateDeltaT::const_iterator sdi = dmi->second.begin();
           sdi != dmi->second.end();
           ++sdi)
        {
          ss << (sdi->first == "" ? "''" : sdi->first) << " -> ";
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

std::string
printNFA (const NFA_conv& nfa)
{
  std::ostringstream ss;

  ss << "automaton " << nfa.name << std::endl;
  for (std::map<SetOfStatesT, StateDeltaT >::const_iterator dmi
         = nfa.delta.begin();
       dmi != nfa.delta.end();
       ++dmi)
    {
      const SetOfStatesT& st = dmi->first;
      if (st == nfa.initial)
        ss << "initial ";
      if (nfa.final.find(st) != nfa.final.end())
        ss << "final ";
      ss << "state " << join_into_stream (st, "-") << " {" << std::endl;
      for (StateDeltaT::const_iterator sdi = dmi->second.begin();
           sdi != dmi->second.end();
           ++sdi)
        {
          ss << sdi->first << " -> "
             << join_into_stream (sdi->second, "-") << std::endl;
        }
      ss << "}" << std::endl;
    }

  return ss.str();
}

std::string
printNFA2dot (const NFA& nfa)
{
  std::ostringstream ss;
  SetOfStatesT final_without_initial (nfa.final);
  final_without_initial.erase (nfa.initial);

  ss << "digraph \"" << nfa.name << "\" {" << std::endl;
  ss << "node [ shape = box"
     << (nfa.final.find (nfa.initial) != nfa.final.end ()
         ? ", color = red "
         : " ") << "]; "
     << "\"" << nfa.initial << "\";" << std::endl;
  ss << "node [ shape = ellipse, color = red ]; ";
  ss << "{ \"" << join_into_stream (final_without_initial, "\" \"") << "\" }";
  ss << ";" << std::endl;
  ss << "node [ shape = ellipse, color = black ];" << std::endl;
  ss << "rankdir = LR;" << std::endl;
  for (DeltaMappingT::const_iterator dmi = nfa.delta.begin();
       dmi != nfa.delta.end();
       ++dmi)
    {
      for (StateDeltaT::const_iterator sdi = dmi->second.begin();
           sdi != dmi->second.end();
           ++sdi)
        {
          ss << "\"" << dmi->first << "\" -> { ";
          ss << "\"" << join_into_stream (sdi->second, "\" \"") << "\"";
          ss << " } [ label = \"" << sdi->first << "\" ];" << std::endl;
        }
    }
  ss << "}" << std::endl;

  return ss.str();
}

std::string
printNFA2vcg (const NFA& nfa)
{
  std::ostringstream ss;

  ss << "graph: {" << std::endl;
  ss << "title: \"" << nfa.name << "\"" << std::endl;
  ss << "display_edge_labels: yes" << std::endl;
  ss << "splines: yes" << std::endl;
  ss << "orientation: left_to_right" << std::endl;
  for (DeltaMappingT::const_iterator dmi = nfa.delta.begin();
       dmi != nfa.delta.end();
       ++dmi)
    {
      ss << "node: { title:\"" << dmi->first << "\" shape: ";
      if (dmi->first == nfa.initial)
        ss << "rhomb ";
      else
        ss << "ellipse ";
      if (nfa.final.find(dmi->first) != nfa.final.end())
        ss << "color: lightred ";
      ss << "}" << std::endl;
    }
  for (DeltaMappingT::const_iterator dmi = nfa.delta.begin();
       dmi != nfa.delta.end();
       ++dmi)
    {
      for (StateDeltaT::const_iterator sdi = dmi->second.begin();
           sdi != dmi->second.end();
           ++sdi)
        {
          for (SetOfStatesT::const_iterator si = sdi->second.begin();
               si != sdi->second.end();
               ++si)
            {
              ss << "edge: { sourcename:\"" << dmi->first << "\" "
                 << "targetname:\"" << *si << "\" "
                 << "label:\"" << sdi->first << "\" }" << std::endl;
            }
        }
    }
  ss << "}" << std::endl;

  return ss.str();
}

/* Get set of input alphabet T from automaton's delta mapping. */
std::set<LetterT >
input_alphabet (const NFA& nfa)
{
  std::set<LetterT> alphabet;
  for (DeltaMappingT::const_iterator i = nfa.delta.begin();
       i != nfa.delta.end();
       ++i)
    {
      for (StateDeltaT::const_iterator k = i->second.begin();
           k != i->second.end();
           ++k)
        {
          alphabet.insert(k->first);
        }
    }
  return alphabet;
}

/* Get automaton's set of states G. */
SetOfStatesT
automaton_states (const NFA& nfa)
{
  SetOfStatesT sos;
  std::transform(nfa.delta.begin(), nfa.delta.end(),
                 std::insert_iterator<SetOfStatesT >(sos, sos.begin()),
                 select1st<DeltaMappingT::value_type >());
  return sos;
}

NFA_conv
convert_NFA2DFA (const NFA& nfa)
{
  NFA_conv nfa_conv;
  /* 1. The set Q' = {{q0}} will be defined, the state {q0} will be treated
     as unmarked. */
  const std::set<LetterT > alphabet (input_alphabet(nfa));
  std::set<SetOfStatesT > Qnew, unmarked, marked;
  SetOfStatesT q0set;
  q0set.insert(nfa.initial);
  Qnew.insert(q0set);
  unmarked.insert(q0set);

  /* 2. If each state in Q' is marked, then continue with step (4). */
  while (! unmarked.empty())
    {
      /* 3. An unmarked state q' will be chosen from Q' and th following
         operations will be executed: */
      SetOfStatesT q_ = *unmarked.begin();
      /* (a) We will determine delta'(q',a) = union(delta(p,a)) for
         p element q' and for all a element T. */
      StateDeltaT stdelta;
      for (std::set<LetterT >::const_iterator letter = alphabet.begin();
           letter != alphabet.end();
           ++letter)
        {
          /* alokace promenne pro sjednoceni union(delta(p,a))
             for p element q' */
          SetOfStatesT* un = new SetOfStatesT;
          for (SetOfStatesT::const_iterator p = q_.begin();
               p != q_.end();
               ++p)
            {
              // najdi StateDeltaT pro stav
              const DeltaMappingT::const_iterator dmi (nfa.delta.find(*p));
              // element nenalezen
              if (dmi == nfa.delta.end())
                throw state_not_found();
              const StateDeltaT& sd (dmi->second);
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
          stdelta.insert(std::make_pair(*letter,*un));
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
      nfa_conv.delta.insert(std::make_pair(q_,stdelta));
    }

  /* 4. q0' = {q0}. */
  nfa_conv.initial = q0set;
  /* 5. F' = {q' : q' element Q', q' intersection F != 0}. */
  for (std::set<SetOfStatesT >::const_iterator q_ = Qnew.begin();
       q_ != Qnew.end();
       ++q_)
    {
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

static
void
next_name (std::string& s)
{
  unsigned long x = 0;
  static const unsigned rng  = 'Z' - 'A' + 1;
  for (std::string::const_iterator ch = s.begin();
       ch != s.end();
       ++ch)
    x = x*rng + (unsigned)(*ch - 'A');
  ++x;
  s.erase(s.begin(), s.end());
  while (x != 0)
    {
      s.insert(s.begin(),(char)('A' + (x % rng)));
      x /= rng;
    }
}

NFA &
rename_states(NFA& nfa)
{
  std::string stname("A");
  std::map<StateT, StateT > table;
  DeltaMappingT::const_iterator dmi;

  /* Vytvor prevodni tabulku jmen stavu. */
  for (dmi = nfa.delta.begin();
       dmi != nfa.delta.end();
       ++dmi)
    {
      table.insert(std::make_pair(dmi->first, stname));
      next_name(stname);
    }

  /* Proved prejmenovani */
  DeltaMappingT newdelta;
  for (dmi = nfa.delta.begin();
       dmi != nfa.delta.end();
       ++dmi)
    {
      StateDeltaT sd;
      for (StateDeltaT::const_iterator sdi = dmi->second.begin();
           sdi != dmi->second.end();
           ++sdi)
        {
          SetOfStatesT s;
          for (SetOfStatesT::const_iterator si = sdi->second.begin();
               si != sdi->second.end();
               ++si)
            s.insert(table.find(*si)->second);
          sd.insert(std::make_pair(sdi->first, s));
        }
      newdelta.insert(std::make_pair(table.find(dmi->first)->second, sd));
    }
  nfa.delta = newdelta;

  /* Prejmenovani pocatecniho a konecnych stavu. */
  nfa.initial = table.find(nfa.initial)->second;
  SetOfStatesT sos;
  for (SetOfStatesT::const_iterator si = nfa.final.begin();
       si != nfa.final.end();
       ++si)
    sos.insert(table.find(*si)->second);
  nfa.final = sos;

  return nfa;
}

NFA
fix_converted (const NFA_conv& nfa_conv, const bool rename)
{
  NFA nfa;
  std::string stname("A");
  std::map<SetOfStatesT, StateT > table;
  std::map<SetOfStatesT, StateDeltaT >::const_iterator dmi;

  /* Vytvor prevodni tabulku jmen stavu. */
  for (dmi = nfa_conv.delta.begin();
       dmi != nfa_conv.delta.end();
       ++dmi)
    {
      if (rename)
        {
          table.insert(std::make_pair(dmi->first, stname));
          next_name(stname);
        }
      else
        table.insert(std::make_pair(dmi->first,
                                    join_seq(std::string("-"),
                                             dmi->first)));
    }

  /* Vytvoreni noveho zobrazeni delta. */
  for (dmi = nfa_conv.delta.begin();
       dmi != nfa_conv.delta.end();
       ++dmi)
    {
      StateDeltaT sd;
      for (StateDeltaT::const_iterator sdi = dmi->second.begin();
           sdi != dmi->second.end();
           ++sdi)
        {
          SetOfStatesT s;
          s.insert(table.find(sdi->second)->second);
          sd.insert(std::make_pair(sdi->first, s));
        }
      nfa.delta.insert(std::make_pair(table.find(dmi->first)->second, sd));
    }

  /* Prejmenovani pocatecniho a konecnych stavu */
  nfa.name = nfa_conv.name;
  nfa.initial = table.find(nfa_conv.initial)->second;
  for (std::set<SetOfStatesT >::const_iterator si = nfa_conv.final.begin();
       si != nfa_conv.final.end();
       ++si)
    nfa.final.insert(table.find(*si)->second);

  return nfa;
}


struct EpsilonClosureData
{
  EpsilonClosureData (SetOfStatesT & closure_ref)
    : closure (closure_ref)
  { }

  SetOfStatesT visited;
  SetOfStatesT & closure;
};


static
void
epsilon_closure (EpsilonClosureData & data, NFA const & nfa,
                 StateT const & state)
{
  if (data.visited.find (state) != data.visited.end ())
    return;

  // Find the state and its delta mapping.
  DeltaMappingT::const_iterator dmi = nfa.delta.find (state);
  if (dmi == nfa.delta.end ())
    throw state_not_found ();
  // Find epsilon transition in the state.
  StateDeltaT::const_iterator sdi = dmi->second.find ("");
  if (sdi != dmi->second.end ())
    {
      // Add all targets of the epsilon transition into closure.
      std::copy (sdi->second.begin (), sdi->second.end (),
                 std::inserter (data.closure, data.closure.begin ()));
      // Mark state as visited.
      data.visited.insert (state);
      // Try to recurse.
      for (SetOfStatesT::const_iterator si = sdi->second.begin ();
           si != sdi->second.end ();
           ++si)
        {
          epsilon_closure (data, nfa, *si);
        }
    }
}


static
void
epsilon_closure (SetOfStatesT & closure, NFA const & nfa, StateT const & start)
{
  EpsilonClosureData data (closure);
  data.closure.insert (start);
  epsilon_closure (data, nfa, start);
}


static
void
copy_non_epsilon_transitions (StateDeltaT & dest, StateDeltaT const & src)
{
  for (StateDeltaT::const_iterator sdi = src.begin (); sdi != src.end ();
       ++sdi)
    {
      if (sdi->first != "")
        {
          StateDeltaT::iterator dest_sdi = dest.find (sdi->first);
          if (dest_sdi != dest.end ())
            std::copy (sdi->second.begin (), sdi->second.end (),
                       std::inserter (dest_sdi->second,
                                      dest_sdi->second.begin ()));
          else
            dest.insert (*sdi);
        }
    }
}


void
remove_epsilons (NFA & nfa)
{
  SetOfStatesT closure;
  for (DeltaMappingT::iterator dmi = nfa.delta.begin ();
       dmi != nfa.delta.end (); ++dmi)
    {
      StateDeltaT::const_iterator sdi = dmi->second.find ("");
      if (sdi != dmi->second.end ())
        {
          // Make epsilon closure.
          closure.clear ();
          epsilon_closure (closure, nfa, dmi->first);
          // Add transitions from source of epsilon transitions to states
          // accessible from destinations of the epsilon transitions... Huh.
          for (SetOfStatesT::const_iterator si = closure.begin ();
               si != closure.end (); ++si)
            {
              DeltaMappingT::iterator source_dmi = nfa.delta.find (*si);
              if (source_dmi == nfa.delta.end ())
                throw state_not_found ();

              copy_non_epsilon_transitions (dmi->second, source_dmi->second);
            }
          dmi->second.erase (sdi->first);
        }
    }
}


NFA
remove_epsilons (NFA const & nfa)
{
  NFA tmp (nfa);
  remove_epsilons (nfa);
  return tmp;
}


struct EqSetRepKey
{
  //! Delta mapping for this equivalency set.
  StateDeltaT const & stdelta;
  //! Flag for equivalency set that contains final states.
  bool const final;

  EqSetRepKey (StateDeltaT const & sd, bool f)
    : stdelta (sd), final (f)
  { }
  friend bool operator == (EqSetRepKey const & el1, EqSetRepKey const & el2);
  friend bool operator < (EqSetRepKey const & el1, EqSetRepKey const & el2);
};

struct state_ptr_lt
{
  inline bool
  operator () (StateT const * s1, StateT const * s2) const
  {
    return *s1 < *s2;
  }
};

void
minimize (NFA & nfa)
{
  while (1)
    {
      //! Zobrazeni delta stavu -> reprezentant tridy ekvivalence.
      typedef std::map<EqSetRepKey, StateT const *> EqMapT;
      EqMapT eqset2rep;
      //! Zobrazeni stav -> reprezentant tridy ekvivalence stavu.
      typedef std::map<StateT const *, StateT const *, state_ptr_lt>
        SubstMapT;
      SubstMapT substmap;

      // Vytvor mnozinu reprezentantu jednotlivych trid ekvivalence
      // a take zobrazeni jednotlivych stavu na reprezentanta sve tridy.
      for (DeltaMappingT::const_iterator dmi = nfa.delta.begin();
           dmi != nfa.delta.end();
           ++dmi)
        {
          std::pair<EqMapT::iterator, bool> p
            = eqset2rep.insert
            (std::make_pair
             (EqSetRepKey (dmi->second,
                           nfa.final.find (dmi->first) != nfa.final.end ()),
              &dmi->first));

          // !p.second == true znamena, ze uz mame reprezentanta pro tento
          // stav a tak jen vlozime zaznam do tabulky substituci.
          if (! p.second)
            substmap.insert (std::make_pair (&dmi->first, p.first->second));
        }

      if (substmap.empty ())
        break;

      // Nove zobrazeni delta po jednom kole minimalizace.
      DeltaMappingT new_delta;

      // Smaz eqvivalentni stavy a proved nahradu za reprezentanta tridy.
      for (DeltaMappingT::const_iterator dmi = nfa.delta.begin ();
           dmi != nfa.delta.end ();
           ++dmi)
        {
          EqMapT::const_iterator ri
            = eqset2rep.find
            (EqSetRepKey (dmi->second,
                          nfa.final.find (dmi->first) != nfa.final.end ()));
          // Pokud to neni reprezentant, tak ho preskocime.
          if (*ri->second != dmi->first)
            nfa.final.erase (dmi->first);
          // Je to reprezentant, tak nahradime jeho vsechny cile prechodu
          // ostatnimy reprezentanty.
          else
            {
              StateDeltaT new_sd;

              // Prochazej vsechny dvojice (pismeno, {mnozina nasledniku}).
              for (StateDeltaT::const_iterator sdi = dmi->second.begin ();
                   sdi != dmi->second.end ();
                   ++sdi)
                {
                  std::pair<StateDeltaT::iterator, bool> p
                    = new_sd.insert (std::make_pair (sdi->first,
                                                     SetOfStatesT ()));
                  SetOfStatesT & target_states = p.first->second;

                  for (SetOfStatesT::const_iterator si = sdi->second.begin ();
                       si != sdi->second.end (); ++si)
                    {
                      SubstMapT::const_iterator const substi
                        = substmap.find (&*si);
                      if (substi == substmap.end ())
                        // Pokud stav neni v substmap, pak je reprezentantem.
                        target_states.insert (*si);
                      else
                        target_states.insert (*substi->second);
                    }
                }

              // Vloz nove vytvorene zobrazeni aktualniho stavu.
              new_delta.insert (std::make_pair (dmi->first, new_sd));
            }
        }
      nfa.delta.swap (new_delta);
    }
}

inline
bool
operator < (EqSetRepKey const & el1, EqSetRepKey const & el2)
{
  if (el1.stdelta < el2.stdelta)
    return true;
  else
    if (el1.stdelta == el2.stdelta)
      return el1.final < el2.final;
    else
      return false;
}

inline
bool
operator == (EqSetRepKey const & el1, EqSetRepKey const & el2)
{
  return el1.final == el2.final && el1.stdelta == el2.stdelta;
}
