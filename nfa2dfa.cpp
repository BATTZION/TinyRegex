#include "nfa2dfa.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include <map>

using namespace std;

typedef vector<Status *> Set;

Set calClosure(Status *status)
{
	Set result;
	queue<Status *> workList;
	workList.push(status);
	result.push_back(status);

	while (!workList.empty()) {
		Status *s = workList.front();
		const vector<Edge *> &outEdges = s->outEdges;
		for (auto edges : outEdges) {
			if (edges->symbol == nullChar) {
				Status *newStatus = edges->end;
				if (find(result.begin(), result.end(), newStatus) == result.end()) {
					result.push_back(newStatus);
					workList.push(newStatus);
				}
			}
		}
		workList.pop();
	}
	return result;
}

void merge(Set &result, Set &temp)
{
	for (auto &x : temp) {
		if (find(result.begin(), result.end(), x) == result.end())
			result.push_back(x);
	}
}

Set calClosureFS(const Set &set)   //calClosure from a set
{
	Set result;
	if (set.size() == 0)
		return result;
	result = calClosure(set[0]);
	for (int i = 1; i < set.size(); i++) {
		Set temp = calClosure(set[i]);
		merge(result, temp);
	}
	return result;
}

bool equal(const Set &a, const Set &b)
{
	if (a.size() != b.size())
		return false;

	for (const auto &x: a) {
		if (find(b.begin(), b.end(), x) == b.end())
			return false;
	}

	for (const auto &y: b) {
		if (find(a.begin(), a.end(), y) == a.end())
			return false;
	}

	return true;
}

int find_set(const vector<Set> &result, const Set &a)
{
	int len = result.size();
	for (int i = 0; i < len; i++) {
		if (equal(result[i], a))
			return i;
	}
	return -1;
}


bool isEndStatus(const Set &set) 
{
	for (const auto &x : set) {
		if (x->finalStatus)
			return true;
	}
	return false;
}

int find(const vector<char> &line, char c)
{
	int len = line.size();
	for (int i = 0; i < len; i++) {
		if (line[i] == c)
			return i;
	}
	return -1;
}

void simulate(const vector<vector<char> > &Dfa, const vector<int> &endStatus)
{
	cout << "input string to match" << endl;
	string s;
	while (cin >> s) {
		int stage = 0;
		int len = s.size();
		for (int i = 0; i < len; i++) {
			int next_stage = find(Dfa[stage], s[i]);
			if (next_stage == -1) {
				cout << "Don't Match!" << endl;
				break;
			}
			stage = next_stage;
		}
		if (find(endStatus.begin(), endStatus.end(), stage) != endStatus.end())
			cout << "Match!" << endl;
		else
			cout << "Don't Match" << endl;
	}
}

Set delta(const Set & set, char c) 
{
	Set result;
	for (const auto &x : set) {
		const vector<Edge *> &outEdges = x->outEdges;
		for (const auto &e : outEdges) {
			if (e->symbol == c)
				result.push_back(e->end);
		}
	}
	return result;
}

void nfa2dfa(const Nfa &nfa, const vector<char> &character)
{
	map<pair<int , int>, char> Map;

	Set q0 = calClosure(nfa.start);

	vector<Set> result;
	queue<Set> workList;

	result.push_back(q0);
	workList.push(q0);
	
	int src_index = -1;
	vector<int> endStatus;
	while (!workList.empty()) {
		++src_index;
		Set q = workList.front();
		if (isEndStatus(q))
			endStatus.push_back(src_index);
		for (auto x : character) {
			Set t = calClosureFS(delta(q, x));
			int des_index = find_set(result, t);
			if (des_index == -1) {
				des_index = result.size();
				result.push_back(t);
				workList.push(t);
			}
			Map[make_pair(src_index, des_index)] = x;
		}
		workList.pop();
	}

	int num_status = result.size();
	vector<vector<char> >Dfa(num_status, vector<char>(num_status));
	auto x = Map.begin();
	while (x != Map.end()) {
		int line = x->first.first;
		int raw = x->first.second;
		char value = x->second;
		Dfa[line][raw] = value;
		x++;
	}
	simulate(Dfa, endStatus);
}



