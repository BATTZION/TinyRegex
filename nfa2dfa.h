#ifndef NFA2DFA_H
#define NFA2DFA_H

#include "nfa.h"
#include <vector>
using std::vector;
std::vector<Status *> calClosure(Status *status);
void nfa2dfa(const Nfa &nfa, const vector<char> &character);

#endif


