#include <iostream>

#include "ast.hh"

std::string ast2str(AstType type) {
	switch (type) {
		case AstType::Global: return "Global";
		case AstType::Include: return "Include";
		case AstType::FuncDec: return "FuncDec";
		case AstType::FuncCall: return "FuncCall";
	}
	
	return "NONE";
}

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
