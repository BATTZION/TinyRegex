#include "parser.h"
#include "nfa.h"
#include "nfa2dfa.h"
#include <iostream>
#include <vector>
using namespace std;

int main()
{
	string s;
	while (getline(cin, s)) {
		try{
			AstNode *root = parser(s);
			Nfa nfa = buildNfa(root);
			vector<char> character = {'a', 'b', 'c', 'd', 'e', 'f'};
			nfa2dfa(nfa, character);
			delete root;
		} catch (runtime_error err) {
			cout << err.what() << endl;
			continue;
		}
	}
}
