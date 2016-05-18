//定义正则表达式语法分析树节点
#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>

enum Type{Char, Op, Error};
struct AstNode {
	AstNode() = default;
	AstNode(Type t, int v, std::string s = ""): type(t), value(v), sValue(s){};
	AstNode(Type t, std::string s): type(t), sValue(s) {}
	AstNode(Type t, int v, AstNode *node): type(t), value(v) { childs.push_back(node); }
	AstNode *addChild(AstNode *child) { childs.push_back(child); }
	Type type;
	int value;
	std::string sValue;
	std::vector<AstNode *> childs;
};

AstNode *parser(const std::string &s);
#endif
