/* 
 * Rule:  Expr
 *      |  ^Expr
 *      |  Expr$
 *
 *  Expr: Expr '|' Cat_Expr
 *      | Cat_Expr
 *
 *  Cat_Expr: Cat_Expr Factor
 *          | Factor
 *
 *  Factor: |Term*
 *          |Term+
 *          |Term?
 *          |Term{n,m}
 *          |Term{n,}
 *          |Term{n}
 *          |Term
 *
 *  Term:   [String]
 *          |[^string]
 *          |.
 *          |Char
 *          |(Expr)
 */

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "parser.h"

using namespace std;


AstNode *parser(const string &s);
AstNode *getExpr(istringstream &stream);
AstNode *getCatExpr(istringstream &stream);
AstNode *getFactor(istringstream &stream);
AstNode *getTerm(istringstream &stream);

// 暂时不处理 \^ 和 \s
AstNode *parser(const string &s)
{
	istringstream stream(s);
	char c;
	AstNode  *result = getExpr(stream);
	if (!stream.eof() || result == NULL) {
		string error;
		stream >> error;
		delete result;
		throw runtime_error("some error happend in " + error);
	}
	if (result->type == Error) {
		string error = result->sValue;
		delete result;
		throw runtime_error(error);
	}
		return result;
}

AstNode *getExpr(istringstream &stream)
{
	AstNode *result;
	char c;
	result = getCatExpr(stream);
	if (result == NULL || result->type == Error || !(stream >> c))
		return result;
	// 创建 | 
	if (c == '|') {
		result = new AstNode(Op, '|', result);
		AstNode *child = getCatExpr(stream);
		if (child == NULL) {
			delete result;
			return new AstNode(Error, "synatx error : |");
		}
		if (child->type == Error) {
			delete result;
			return child;
		}
		result->addChild(child);
	}
	else {
		stream.putback(c);
		return result;
	}
	while (stream >> c) {
		if (c == '|') {
			AstNode *otherChild = getCatExpr(stream);
			if (otherChild == NULL) {
				delete result;
				return new AstNode(Error, "synatx error : |");
			}
			if (otherChild->type == Error) {
				delete result;
				return otherChild;
			}
			result->addChild(otherChild);
		}
		else {
			stream.putback(c);
			return result;
		}
	}
	return result;
}

AstNode *getCatExpr(istringstream &stream)
{
	AstNode *result = getFactor(stream);
	if (result == NULL || result->type == Error)
		return result;
	AstNode *child = getFactor(stream);
	if (child == NULL)
		return result;
	if (child->type == Error) {
		delete result;
		return child;
	}
	result = new AstNode(Op, '-', result);
	result->addChild(child);
	while (true) {
		AstNode *child = getFactor(stream);
		if (child == NULL)
			break;
		if (child->type == Error) {
			delete result;
			return child;
		}
		result->addChild(child);
	}
	return result;
}

bool isDigit(char c) { return ('0' <= c && c <= '9'); }

AstNode *getOpRep(istringstream &stream)
{
	char c;
	stream >> c;
	string start, end;
	while (stream >> c && isDigit(c)) 
		start += c;
	if (start.size() == 0)
		return new AstNode(Error, "synatx error: {}");
	if (c == '}')
		end = "";
	else if(c == ',') {
		stream >> c;
		if (c == '}')
			end = "$";
		else {
			if (!isDigit(c))
				return new AstNode(Error, "synatx error: {}");
			end += c;
			while (stream >> c && isDigit(c)) 
				end += c;
			if (c != '}')
				return new AstNode(Error, "synatx error: missing '}'");
		}
	}
	else 
		return new AstNode(Error, "synatx error: {}");
	string sVaule = start + ":" + end;
	return new AstNode(Op, '{',  sVaule);
}

AstNode *getFactor(istringstream &stream)
{
	char c;
	AstNode *result = getTerm(stream);
	if (result == NULL || result->type == Error || !(stream >> c))
		return result;
	if (c == '*' || c == '+' || c == '?')
		return new AstNode(Op, c, result);
	if (c == '{') {
		stream.putback(c);
		AstNode *op = getOpRep(stream);
		if (op->type == Error) {
			delete result;
			return op;
		}
		op->addChild(result);
		return op;
	}
	stream.putback(c);
	return result;
}
AstNode *getOpRange(istringstream &stream)
{
	char c;
	string val;
	while (stream >> c && c != ']') {
		val += c;
	}
	if (c != ']')
		return new AstNode(Error, "synatx error: missing ']'");
	return new AstNode(Op, '[', val);
}
AstNode *getTerm(istringstream &stream)
{
	char c;
	if(!(stream >> c))
		return NULL;
	switch (c) {
		case '[':
			return  getOpRange(stream);
		case '(': {
			AstNode *expr = getExpr(stream);
			if ((stream >> c) && (c == ')'))
				return expr;
			delete expr;
			return new AstNode(Error, "synatx error: missing ')'");
				  }
		case '\\':
			if ((stream >> c)) {
				AstNode *symbol = new AstNode(Char, c);
				AstNode *result = new AstNode(Op, '\\', symbol);
				return result;
			}
			return new AstNode(Error, "synatx error: \\");
		case '.':
			return new AstNode(Op, c);
		case '+':
		case '*':
		case ']':
		case '}':
		case '?':
		case ')':
		case '|':
			stream.putback(c);
			return NULL;
		default:
			return new AstNode(Char, c);
	}
}







