#pragma once

#include <string>
#include <vector>

#include <ast.hh>

#include "ltac.hh"

class LTAC_Builder {
public:
	explicit LTAC_Builder(std::string output);
	void assemble(AstNode *top);
	
	//Function stuff
	void build_extern(AstNode *node);
	void build_func_dec(AstNode *node);
	
	//Returns the pointer to the abstract file
	LtacFile *get_file() {
		return file;
	}
private:
	LtacFile *file;
};
