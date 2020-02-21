#pragma once

#include <map>
#include <string>

#include "ltac.hh"
#include "ast.hh"

class LTAC_Builder {
public:
	LTAC_Builder() {}
	LtacFile *build_file(AstNode *top);
protected:
	void assemble(AstNode *top);
	
	//Function stuff
	void build_func(AstNode *node, bool is_extern=false);
	void build_func_call(AstNode *node);
	void check_overload();
	
	//Variable stuff
	void build_var_dec(AstNode *node);
	void build_var_assign(AstNode *node);
	
	//Utility stuff
	LtacNode *build_string(AstNode *node);
	void inc_stack(DataType type);
	
	LtacFile *file;
private:
	//Variable related stuff
	int stack_pos = 0;
	std::map<std::string, Var> vars;
	
	//String stuff
	int str_count = 0;
	std::map<std::string, std::string> dec_strings;
};
