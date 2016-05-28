#include "nfa2dfa.h"
#include "parser.h"
#include <iostream>
#include <vector>
#include <string>


using namespace std;

int main()
{
	string s = "a(b|c)*";
	AstNode *tree = parser(s);
	Nfa nfa = buildNfa(tree);
	vector<Status *> result =  calClosure(nfa.start->outEdges[0]->end->outEdges[0]->end->outEdges[0]->end);
	cout << result.size() << endl;
	return 0;
}
