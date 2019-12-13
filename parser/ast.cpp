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
	std::cout << ast2str(node->type);
	
	if (node->type == AstType::Include) {
		std::cout << " [" 
			<< dynamic_cast<AstInclude *>(node)->get_include()
			<< "]";
	} else if (node->type == AstType::FuncDec) {
		std::cout << " [" 
			<< dynamic_cast<AstFuncDec *>(node)->get_name()
			<< "]";
	} else if (node->type == AstType::FuncCall) {
		AstFuncCall *fc = dynamic_cast<AstFuncCall *>(node);
		std::cout << " [" << fc->get_name() << "] <";
		for (auto a : fc->get_args()) {
			std::cout << "{" << a.id << "}";
		}
		std::cout << ">";
	}
	
	std::cout << std::endl;

	if (node->children.size() > 0) {
		for (auto c : node->children) {
			print_tree(c, indent+1);
		}
	}
}
