#include "parser.h"
#include "nfa.h"
#include <vector>
#include <iostream>

using namespace std;

const int nullChar = 256; // 表示空字符


Nfa opChar(AstNode *node) 
{
	int symbol = node->value;
	Status *start = new Status(false);
	Status *end = new Status(true);
	Edge *edge = new Edge(symbol, start, end);
	start->outEdges.push_back(edge);
	end->inEdges.push_back(edge);
	Nfa result(start, end);
	return result;
}

void connectNfa(const Nfa &a, const Nfa &b)
{
	Edge *edge = new Edge(nullChar, a.end, b.start);
	a.end->finalStatus = false;
	a.end->outEdges.push_back(edge);
	b.start->inEdges.push_back(edge);
}

void connectStatus(Status *start, Status *end)
{
	Edge *edge = new Edge(nullChar, start, end);
	start->outEdges.push_back(edge);
	end->inEdges.push_back(edge);
	start->finalStatus = false;
}

Nfa opAnd(vector<AstNode *> &childs)
{
	int childNum = childs.size();
	vector<Nfa> nfas(childNum);
	for (int i = 0; i < childNum; i++) 
		nfas[i] = buildNfa(childs[i]);
	for (int i = 1; i < childNum; i++) 
		connectNfa(nfas[i-1], nfas[i]);
	Nfa result(nfas[0].start, nfas[childNum-1].end);
	return result;
}

Nfa opOr(vector<AstNode *> &childs)
{
    int childNum = childs.size();
	Nfa temp;
	Status *start = new Status(false);
	Status *end = new Status(true);
	for (int i = 0; i < childNum; i++) {
		temp = buildNfa(childs[i]);
		connectStatus(start, temp.start);
		connectStatus(temp.end, end);
	}
	Nfa result(start, end);
	return result;
}

Nfa opRepetive(AstNode *root)
{
	Nfa nfa = buildNfa(root);
	Status *start = new Status(false);
	Status *end = new Status(true);
	connectStatus(nfa.end, nfa.start);
	connectStatus(start, nfa.start);
	connectStatus(start, end);
	connectStatus(nfa.end, end);
	Nfa result(start, end);
	return result;
}

Nfa opOption(AstNode *root)
{
	Nfa nfa = buildNfa(root);
	connectStatus(nfa.start, nfa.end);
	return nfa;
}

Nfa buildNfa(AstNode *root) 
{  
	Nfa result;
	Type type = root->type;
	if (type == Char) {
		result = opChar(root);
		return result;
	}
	if (type == Op) {
		int c = root->value;
		switch (c) {
			case '-': //串联符号
				result = opAnd(root->childs);
				break;
			case '|':
				result = opOr(root->childs);
				break;
			case '*':
				result = opRepetive(root->childs[0]);
				break;
			case '?':
				result = opOption(root->childs[0]);
				break;
		}
	}
	return result;
}

