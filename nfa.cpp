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

Nfa opChar(int symbol) 
{
	Status *start = new Status(false);
	Status *end = new Status(true);
	Edge *edge = new Edge(symbol, start, end);
	start->outEdges.push_back(edge);
	end->inEdges.push_back(edge);
	Nfa result(start, end);
	characters.push_back(symbol);
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
	Nfa nfa_temp2 = opRepetive(root);
	connectNfa(nfa_temp, nfa_temp2);
	Nfa result(nfa_temp.start, nfa_temp2.end);
	return result;
}

//删除重复元素
void uniqueVector(vector<int> &set)
{
	auto x = set.begin();
	map<int, int> Maps;
	while (x != set.end()) {
		if (Maps.find(*x) != Maps.end()) {
			x = set.erase(x);
		}
		else {
			Maps[*x] = 1;
			x++;
		}
	}
}
vector<int> getSet(const string &str) 
{
	vector<int> result;
	if (str[0] != '^') {
		int l = str.size(), i = 0;
		while (i < l) {
			if (i + 1 < l && str[i] != '-' && str[i + 1] != '-') {
				result.push_back(str[i++]);
				continue;
			}
			if (i + 1 >= l) {
				result.push_back(str[i]);
				break;
			}
			if (str[i+1] == '-') {
				for (int j = str[i]; j <= str[i+2]; j++) {
					result.push_back(j);
				}
				i += 3;
			}
		}
	}
	else {
		string s;
		for (int i = 1; i < str.size(); i++)
			s += str[i];
		vector<int> temp = getSet(s);
		for (int i = 1; i < 255; i++) {
			if (find(temp.begin(), temp.end(), i) == temp.end())
				result.push_back(i);
		}
	}
	uniqueVector(result);
	return result;
} 
Nfa opRange(const string &range)
{
	vector<AstNode *> childs;
	vector<int> charSet = getSet(range);
	for (auto x : charSet) {
		AstNode *temp = new AstNode(Char, x);
		childs.push_back(temp);
		characters.push_back(x);
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

Nfa opEscape(AstNode *root)
{
	int c = root->value;
	switch (c) {
		case 'n':
			return opChar('\n');
		default:
			return opChar(c);
			break;
	}
}

Nfa buildNfa(AstNode *root) 
{
	Nfa result;
	Type type = root->type;
	if (type == Char) {
		result = opChar(root->value);
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
				result = opChar(anyChar);
				break;
			case '+':
				result = opPlus(root->childs[0]);
				break;
			case '[':
				result = opRange(root->sValue);
				break;
			case '{':
				result = opRepetiveRange(root);
				break;
			case '\\':
				result = opEscape(root->childs[0]);
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

















