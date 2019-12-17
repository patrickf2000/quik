#include <iostream>
#include <cstdlib>

#include "syntax.hh"
#include "parse.hh"

//Checks the global scope for invalid arguments
// Only function declarations and variable declarations are allowed
// on the global scope
void SyntaxCheck::check_global(AstNode *top) {
	if (top->type != AstType::Scope) {
		return;
	}
	
	for (auto node : top->children) {
		switch (node->type) {
			case AstType::FuncDec:
			case AstType::ExternFunc:
			case AstType::VarDec: 
			case AstType::EndIf: continue;
		}
			
		Error er;
		er.ln = node->ln;
		er.msg = "Invalid token on the global scope.";
		errors.push_back(er);
	}
}

//Checks to see if we have any errors and prints them
void SyntaxCheck::evaluate() {
	if (errors.size() == 0)
		return;
		
	for (auto err : errors) {
		syntax_error(err.ln, err.msg, false);
		std::cout << std::endl;
	}
	
	std::exit(1);
}
