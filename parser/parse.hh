#pragma once

#include <vector>
#include <lex.hh>

#include "ast.hh"

void syntax_error(Line ln, std::string msg, bool exit = true);
DataType ttype2dtype(TokenType t);

class QkParser {
public:
	AstNode *build_node(Line ln);
	AstNode *build_id(Line ln);
	int build_tree(std::vector<AstNode *> nodes, AstNode *top, int index = 0, bool in_if = false);
	
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
	
	//Arrays and structures
	AstArrayDec *build_array(Line ln, bool is_float);
	AstStructDec *build_struct_dec(Line ln);
	AstStruct *build_struct_var(Line ln);
	
	void analyze_and_check(AstScope *top);
protected:
	//Analyzer functions
	void find_variables(AstNode *top);
	void find_assign(AstNode *top, AstScope *scope);
	void find_cond(AstNode *top);
	void check_return(AstNode *top);
};
