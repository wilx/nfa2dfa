
#include <set>
#include <map>
#include <utility>

typedef char StateT;
typedef char LetterT;

struct StateDelta {
    std::map<LetterT, StateT > delta;
};

int main (int argc, char* argv[])
{
    std::map<StateT, StateDelta > nfa;

    
}
