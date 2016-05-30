#ifndef NFA_H
#define NFA_H

#include <vector>
#include "parser.h"

const int nullChar = 256;   //空字符
const int anyChar =  257;    //任何字符

struct Status;
struct Edge {
	int symbol;
	Status *start = NULL;
	Status *end = NULL;
	Edge() = default;
	Edge(int c, Status *s, Status *e): symbol(c), start(s), end(e) {}
};

struct Status {
	//static int count;
	//int id;
	std::vector<Edge *> inEdges;
	std::vector<Edge *> outEdges;
	bool finalStatus;
	Status(bool fin = false):finalStatus(fin) { /*id = count++;*/ }
	//~Status() { count--; }
};

struct Nfa {
	Status *start = NULL;
	Status *end = NULL;
	Nfa() = default;
	Nfa(Status *s, Status *e):start(s), end(e) {}
};

Nfa buildNfa(AstNode *root);
int deleteNfa(Nfa &nfa);
void release(Nfa &nfa);
#endif
