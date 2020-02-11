#pragma once

#include <string>
#include <map>

#include <ast.hh>

#include "ltac.hh"

class LTAC_Builder {
public:
	explicit LTAC_Builder(AsmFile *s) {
		scope = s;
	}
	
	void build(AstNode *top);
	
	//Function stuff
	void build_extern(AstNode *node);
	void build_func(AstNode *node);
	void build_func_call(AstNode *node);
	void build_ret(AstNode *node);
	
	//Variable stuff
	void build_var_dec(AstNode *node);
	void build_var_assign(AstNode *node);
private:
	AsmFile *scope;
	AstScope *var_scope;
	std::map<std::string, Var> vars;
	
	//Control stuff
	int str_index = 0;
	int stack_pos = 4;
};

//The debug function
std::string ltac_translate(AsmNode *node);

//Build and write functions
AsmFile *build_asm_file(AstNode *top);
void write_asm_file(AsmFile *file);
