#pragma once

#include <vector>
#include <string>

enum AstType {
	Global,
	Include,
	FuncDec,
	FuncCall
};

class AstNode {
public:
	explicit AstNode();
	explicit AstNode(AstType t) { type = t; }
	AstType type;
	std::vector<AstNode *> children;
};

std::string ast2str(AstType type);
void print_tree(AstNode *node, int indent = 0);
