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
	void build_func(AstNode *node);
	void build_func_call(AstNode *node);
	void build_var_dec(AstNode *node);
	void build_var_assign(AstNode *node);
	LtacNode *build_string(AstNode *node);
	
	LtacFile *file;
private:
	//Variable related stuff
	int stack_pos = 0;
	std::map<std::string, Var> vars;
	
	//Other stuff
	int str_count = 0;
};
