#include <iostream>

#include "ast.hh"

//The include AST type
AstInclude::AstInclude() {
	type = AstType::Include;
}

AstInclude::AstInclude(std::string path) {
	include = path;
	type = AstType::Include;
}

std::string AstInclude::get_include() {
	return include;
}

void AstInclude::set_include(std::string path) {
	include = path;
}

//Returns an AST type as a string (debugging purposes)
std::string ast2str(AstType type) {
	switch (type) {
		case AstType::Global: return "Global";
		case AstType::Include: return "Include";
		case AstType::FuncDec: return "FuncDec";
		case AstType::FuncCall: return "FuncCall";
	}
	
	return "NONE";
}

//Prints an AST tree
void print_tree(AstNode *node, int indent) {
	for (int i = 0; i<indent; i++) {
		std::cout << "  ";
	}
	std::cout << ast2str(node->type) << std::endl;

	if (node->children.size() > 0) {
		for (auto c : node->children) {
			print_tree(c, indent+1);
		}
	}
}
