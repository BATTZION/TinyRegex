#include "parser.h"
#include "nfa.h"
#include <iostream>

using namespace std;

int main()
{
	string s;
	while (getline(cin, s)) {
		try{
			AstNode *root = parser(s);
			Nfa nfa = buildNfa(root);
			delete root;
		} catch (runtime_error err) {
			cout << err.what() << endl;
			continue;
		}
	}
}
