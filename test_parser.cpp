#include "parser.h"
#include <iostream>
#include <queue>

using namespace std;
void printNode(AstNode *node) {
	int type = node->type;
	switch (type) {
		case 0:
			cout << "( Char, " << char(node->value) << " ) ";
			break;
		case 1:
			if (node->value == '[') 
				cout << "( Op, range: " << node->sValue << " ) ";
			else if (node->value == '{')
				cout << "( Op, Repe: " << node->sValue << " ) ";
			else if (node->value == '@')
				cout << "( Op, and ) ";
			else
				cout <<"( Op, " << char(node->value) << " )";
			break;
		default:
			break;
	}
}
void PrintAst(AstNode *root)
{
	if (root == NULL)
		return;
	queue<AstNode *> Queue;
	Queue.push(root);
	int num_son = 1, temp = 0;
	while (!Queue.empty()) {
		while (num_son-- != 0) {
			AstNode *node = Queue.front(); 
			printNode(node);
			for (auto &x : node->childs) {
				Queue.push(x);
				++temp;
			}
			Queue.pop();
		}
		cout << endl;
		num_son = temp;
		temp = 0;
	}
}


int main()
{
	string s;
	while (getline(cin, s)) {
		try{
			PrintAst(parser(s));
		} catch (runtime_error err) {
			cout << err.what() << endl;
			continue;
		}
	}
	return 0;
}
