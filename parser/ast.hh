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
	explicit AstNode() {}
	explicit AstNode(AstType t) { type = t; }
	AstType type;
	std::vector<AstNode *> children;
};

class AstInclude : public AstNode {
public:
	explicit AstInclude();
	explicit AstInclude(std::string path);
	std::string get_include();
	void set_include(std::string path);
private:
	std::string include;
};

std::string ast2str(AstType type);
void print_tree(AstNode *node, int indent = 0);
