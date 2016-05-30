#ifndef NFA2DFA_H
#define NFA2DFA_H

#include "nfa.h"
#include <vector>
#include <map>

using std::map;
using std::vector;
using std::pair;

vector<Status *> calClosure(Status *status);
struct Dfa {
	typedef map<pair<int, int>, int> Map;
	Map graph;
	vector<int> endStatus;
	Dfa(Map g, vector<int> es): graph(g), endStatus(es) {}
};
Dfa nfa2dfa(const Nfa &nfa, const vector<int> &character);
bool simulateDfa(const std::string &str, Dfa &dfa);


#endif

