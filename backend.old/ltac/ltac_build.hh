#pragma once

#include <string>
#include <vector>
#include <map>
#include <stack>

#include <ast.hh>

#include "ltac.hh"

class LTAC_Builder {
public:
	explicit LTAC_Builder() { }
	explicit LTAC_Builder(std::string output);
	void assemble(AstNode *top);
	
	//Function stuff
	void build_extern(AstNode *node);
	void build_func_dec(AstNode *node);
	void build_func_call(AstNode *node);
	void build_ret(AstNode *node);
	
	//Variable functions
	void build_var_dec(AstNode *node);
	void build_var_assign(AstNode *node);
	void build_var_assign(AstVarDec *va, Var v);
	
	//Flow control
	void build_conditional(AstNode *node);
	
	//Returns the pointer to the abstract file
	LtacFile *get_file() {
		return file;
	}
protected:
	//Utility functions
	int size_of(DataType t);
private:
	LtacFile *file;
	
	//Variable stuff
	std::map<std::string, Var> vars;
	int stack_pos = 0;
	
	//Position control
	int str_lbl = 0;
	
	//Label control stuff
	int lbl_count = 0;
	std::stack<std::string> top_labels;
	std::stack<std::string> labels;
};
