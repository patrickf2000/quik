#include <iostream>

#include "ast.hh"

//Returns an AST type as a string (debugging purposes)
std::string ast2str(AstType type) {
	switch (type) {
		case AstType::Global: return "Global";
		case AstType::Include: return "Include";
		case AstType::FuncDec: return "FuncDec";
		case AstType::FuncCall: return "FuncCall";
		case AstType::End: return "End";
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
