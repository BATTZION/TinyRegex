#ifndef NFA_H
#define NFA_H

#include <vector>
struct Status;
struct Edge {
	int symbol;
	Status *start = NULL;
	Status *end = NULL;
	Edge() = default;
	Edge(int c, Status *s, Status *e): symbol(c), start(s), end(e) {}
};

struct Status {
	std::vector<Edge *> inEdges;
	std::vector<Edge *> outEdges;
	bool finalStatus;
	Status() = default;
	Status(bool fin):finalStatus(fin) {}
};

struct Nfa {
	Status *start = NULL;
	Status *end = NULL;
	Nfa() = default;
	Nfa(Status *s, Status *e):start(s), end(e) {}
};
Nfa buildNfa(AstNode *root);
#endif
