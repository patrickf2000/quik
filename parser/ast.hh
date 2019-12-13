#pragma once

#include <vector>
#include <string>

enum AstType {
	Global,
	Include,
	FuncDec,
	FuncCall,
	End
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

//The function declaration type
//TODO: Arguments
class AstFuncDec : public AstNode {
public:
	explicit AstFuncDec() { type = AstType::FuncDec; }
	explicit AstFuncDec(std::string n) {
		type = AstType::FuncDec;
		name = n;
	}
	
	std::string get_name() { return name; }
	void set_name(std::string n) { name = n; }
private:
	std::string name = "";
};

std::string ast2str(AstType type);
void print_tree(AstNode *node, int indent = 0);
