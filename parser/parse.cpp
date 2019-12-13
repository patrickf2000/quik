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
		
	//Build a function declaration node
	//TODO: We need arguments
	} else if (first.type == TokenType::FUNC_DEC) {
		if (tokens.size() < 2) {
			std::cout << "Syntax element: Invalid size for function declaration." << std::endl;
			std::exit(1);
		}
		
		Token id = tokens.at(1);
		if (id.type != TokenType::ID) {
			std::cout << "Syntax Error: Second element of an include statement "
				<< "should be an ID." << std::endl;
			std::exit(1);
		}
		
		AstFuncDec *fd = new AstFuncDec(id.id);
		return fd;
	
	//Build an End statement
	} else if (first.type == TokenType::END) {
		AstNode *node = new AstNode(AstType::End);
		return node;
		
	//Handle if the first node is an ID
	} else if (first.type == TokenType::ID) {
		if (tokens.size() < 2) {
			std::cout << "Invalid syntax: ID only" << std::endl;
			std::exit(1);
		}
		
		//Build a function call
		if (tokens.at(1).type == TokenType::LEFT_PAREN) {
			std::string name = first.id;
			std::vector<Token> args;
			
			for (int i = 2; i<tokens.size(); i++) {
				auto t = tokens.at(i);
			
				if (t.type == TokenType::RIGHT_PAREN) {
					break;
				} else if (t.type == TokenType::COMMA) {
					continue;
				}
				
				args.push_back(t);
			}
			
			AstFuncCall *call = new AstFuncCall(name, args);
			return call;
			
		//Build an assignment
		} else if (tokens.at(1).type == TokenType::ASSIGN) {
			std::cout << "VAR!" << std::endl;
		}
	}

	return nullptr;
}
