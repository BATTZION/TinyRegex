#include "parser.h"
#include "nfa.h"
#include "nfa2dfa.h"
#include <iostream>
#include <vector>
using namespace std;
extern vector<int> characters;
int main(int argc, char **argv)
{
	string s, str;
	getline(cin, s);
	try{
		AstNode *root = parser(s);
		Nfa nfa = buildNfa(root);
		Dfa dfa = nfa2dfa(nfa, characters);
		while (getline(cin, str)) {
			if (simulateDfa(str, dfa)) 
				cout << "Match!" << endl;
			else 
				cout << "Don't Match!" << endl;
		}
		delete root;
		characters.clear();
		release(nfa);
	} catch (runtime_error err) {
		cout << err.what() << endl;
	}	

}
