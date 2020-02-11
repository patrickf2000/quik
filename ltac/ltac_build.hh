#pragma once

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
private:
	AsmFile *scope;
	
	//Control stuff
	int str_index = 0;
};

//The debug function
std::string ltac_translate(AsmNode *node);
