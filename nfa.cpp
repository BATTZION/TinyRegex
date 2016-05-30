#include "parser.h"
#include "nfa.h"
#include <vector>
#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <algorithm>

using namespace std;

//int Status::count = 0;

vector<int> characters;

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

Nfa opAnd(const vector<AstNode *> &childs)
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

Nfa opOr(const vector<AstNode *> &childs)
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

Nfa opPlus(AstNode *root)
{
	Nfa nfa_temp = buildNfa(root);
	Nfa nfa_temp2 = buildNfa(root);
	connectNfa(nfa_temp, nfa_temp2);
	return Nfa(nfa_temp.start, nfa_temp2.end);
}

Nfa opRange(AstNode *root)
{
	vector<AstNode *> childs;
	string range = root->sValue;
	if (range[0] != '^') {
		if (range[1] == '-') {
			cout << range[0] << range[2] << endl;
			for (int i = range[0]; i <= range[2]; i++) {
				AstNode *temp = new AstNode(Char, i);
				childs.push_back(temp);
			}
		}
		else {
			for (auto x : range) {
				AstNode *temp = new AstNode(Char, x);
				childs.push_back(temp);
			}
		}
	}
	else {
		if (range[2] == '-') {
			for (int i = 1; i < 255; i++) {
				if (range[1] <= i && i <= range[3])
					continue;
				AstNode *temp = new AstNode(Char, i);
				childs.push_back(temp);
			} 
		}
		else {
			for (int i = 1; i < 255; i++) {
				if (range.find(i) != string::npos)
					continue;
				AstNode *temp = new AstNode(Char, i);
				childs.push_back(temp);
			}
		}
	}
	return opOr(childs);
}

Nfa opRepetiveRange(AstNode *root)
{
	string range = root->sValue;
	int begin = 0, end = 0, i = 0;
	vector<AstNode *> childs;
	for (i = 0; range[i] != ':'; i++) 
		begin += range[i] - '0';
	if (i == (range.size() - 1))
		end = begin * 2;
	else if (range[++i] == '$')
		end = -1;
	else  
		for (; i < range.size(); i++)
			end += range[i] - '0';
	
	if (end != -1) {
		for (int i = 0; i < begin; i++) {
			childs.push_back(root->childs[0]);
		}
		for (int i = begin; i <= end; i++) {

			childs.push_back(root->childs[0]);
		}
		return opAnd(childs);
	}
	else {
		for (int i = 0; i < begin; i++) 
			childs.push_back(root->childs[0]);
		Nfa nfa_temp = opAnd(childs);
		Nfa nfa_temp2 = opRepetive(root->childs[0]);
		connectNfa(nfa_temp, nfa_temp2);
		return Nfa(nfa_temp.start, nfa_temp2.end);
	}
}

Nfa buildNfa(AstNode *root) 
{
	Nfa result;
	Type type = root->type;
	if (type == Char) {
		characters.push_back(root->value);
		result = opChar(root);
		return result;
	}
	if (type == Op) {
		int c = root->value;
		switch (c) {
			case '@': //串联符号
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
			case '.':
				result = opChar(root);
				break;
			case '+':
				result = opPlus(root->childs[0]);
				break;
			case '[':
				result = opRange(root);
				break;
			case '{':
				result = opRepetiveRange(root);
				break;
		}
	}
	return result;
}

void release(Nfa &nfa)
{
	vector<Status *> result;
	queue<Status *> workList;

	result.push_back(nfa.start);
	workList.push(nfa.start);

	while (!workList.empty()) {
		Status *temp = workList.front();
		vector<Edge *> &outEdges = temp->outEdges;
		for (auto x : outEdges) {
			Status *next_status = x->end;
			if (find(result.begin(), result.end(), next_status) == result.end()) {
				result.push_back(next_status);
				workList.push(next_status);
			}
			delete x;
		}
		workList.pop();
	}

	for (auto x : result)
		delete x;
}

















