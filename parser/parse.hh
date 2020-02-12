#pragma once

#include <vector>
#include <lex.hh>

#include "ast.hh"

void syntax_error(Line ln, std::string msg, bool exit = true);
DataType ttype2dtype(TokenType t);
int build_tree(std::vector<AstNode *> nodes, AstNode *top, int index = 0, bool in_if = false);

class QkParser {
public:
	AstNode *build_node(Line ln);
	
	//Function stuff
	AstFuncDec *build_func_dec(Line ln);
	AstFuncCall *build_func_call(Line ln);
	AstReturn *build_ret(Line ln);
	
	//Variables
	AstVarDec *basic_var_dec(Line ln);
	void build_var_parts(AstNode *vd, int start, std::vector<Token> tokens);
	
	//Conditionals
	AstCond *build_conditional(Line ln);
	
	//Loops
	AstLoop *build_loop(Line ln);
	AstForEach *build_foreach(Line ln);
};
