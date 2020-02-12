#pragma once

#include <string>
#include <vector>

#include <ast.hh>

#include "ltac.hh"

class LTAC_Builder {
public:
	explicit LTAC_Builder(std::string output);
	void assemble(AstNode *top);
	
	//Returns the pointer to the abstract file
	LtacFile *get_file() {
		return file;
	}
private:
	LtacFile *file;
};
