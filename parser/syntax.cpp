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

//Scans from all the scopes, and makes sure there are no undeclared variables
void SyntaxCheck::check_vars(AstNode *top, std::map<std::string, Var> vars) {
	if (top->type == AstType::Scope) {
		AstScope *scope = static_cast<AstScope *>(top);
		vars = scope->vars;
	}
	
	for (auto node : top->children) {
		if (node->type == AstType::VarAssign || node->type == AstType::Id) {
			AstAttrNode *va = static_cast<AstAttrNode *>(node);
			if (vars.find(va->get_name()) == vars.end()) {
				Error er;
				er.ln = node->ln;
				er.msg = "Use of undeclared variable.";
				errors.push_back(er);
			}
		}
		
		if (node->children.size() > 0) {
			check_vars(node, vars);
		}
	}
}

//Checks to see if we have any errors and prints them
void SyntaxCheck::evaluate(bool fail) {
	if (errors.size() == 0)
		return;
		
	for (auto err : errors) {
		syntax_error(err.ln, err.msg, false);
		std::cout << std::endl;
	}
	
	if (fail)
		std::exit(1);
}
