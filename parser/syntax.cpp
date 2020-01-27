#include <iostream>
#include <cstdlib>

#include "syntax.hh"
#include "parse.hh"

std::vector<Error> errors;

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

//Scans the tree to check for invalid variable types in the loop keyword
void SyntaxCheck::check_lp_vars(AstNode *top, std::map<std::string, Var> vars) {
	if (top->type == AstType::Scope) {
		AstScope *scope = static_cast<AstScope *>(top);
		vars = scope->vars;
	}
	
	for (auto node : top->children) {
		if (node->type == AstType::Loop) {
			AstLoop *lp = static_cast<AstLoop *>(node);
			AstAttrNode *va = static_cast<AstAttrNode *>(lp->param);
			
			if (vars.find(va->get_name()) == vars.end()) {
				Error er;
				er.ln = node->ln;
				er.msg = "Use of undeclared variable.";
				errors.push_back(er);
			} else {
				Var v = vars[va->get_name()];
				if (v.type != DataType::Int) {
					Error er;
					er.ln = node->ln;
					er.msg = "Only integer variables may be used with the loop statement.";
					errors.push_back(er);
				}
			}
		}
		
		if (node->children.size() > 0) {
			check_lp_vars(node, vars);
		}
	}
}

//Prints the actual syntax error
void SyntaxCheck::syntax_error(Error err) {
	std::cout << "Syntax Error: " << err.msg << std::endl;
	std::cout << "[" << err.ln.no << "] " << err.ln.original << std::endl;
}

//Checks to see if we have any errors and prints them
void SyntaxCheck::evaluate(bool fail) {
	if (errors.size() == 0)
		return;
		
	for (auto err : errors) {
		syntax_error(err);
		std::cout << std::endl;
	}
	
	if (fail)
		std::exit(1);
}
