#include <iostream>
#include <cstdlib>

#include "parse.hh"

//Builds an AST node from a string of tokens
AstNode *build_node(std::vector<Token> tokens) {
	auto first = tokens.at(0);
	
	//Build an include node
	if (first.type == TokenType::INCLUDE) {
		if (tokens.size() != 2) {
			std::cout << "Error at include: invalid size." << std::endl;
			std::exit(1);
		}
		
		Token id = tokens.at(1);
		if (id.type != TokenType::ID) {
			std::cout << "Syntax Error: Second element of an include statement "
				<< "should be an ID." << std::endl;
			std::exit(1);
		}
	
		AstInclude *node = new AstInclude(id.id);
		return node;
	}

	return nullptr;
}
