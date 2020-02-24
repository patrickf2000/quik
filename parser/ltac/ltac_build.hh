#pragma once

#include <map>
#include <stack>
#include <string>

#include "ltac.hh"
#include "ast.hh"

class LTAC_Builder {
public:
	LTAC_Builder() {}
	void set_ptr_size(int size) { ptr_size = size; }
	LtacFile *build_file(AstNode *top);
protected:
	void assemble(AstNode *top);
	
	//Function stuff
	LtacFunc *build_func(AstNode *node, bool is_extern=false);
	LtacFuncCall *build_func_call(AstNode *node);
	void build_ret(AstNode *node);
	
	//Variable stuff
	void build_var_dec(AstNode *node);
	void build_var_assign(AstNode *node);
	void build_multi_var_assign(AstNode *node);
	LtacNode *convert_ast_var(AstNode *val);
	
	//Utility stuff
	LtacNode *build_string(AstNode *node);
	LtacNode *build_float(AstNode *node);
	void inc_stack(DataType type);
	
	//Comparisons
	DataType determine_type(LtacNode *node);
	void build_cmp(AstNode *node, bool is_loop = false);
	
	LtacFile *file;
private:
	//Variable related stuff
	int stack_pos = 0;
	int ptr_size = 8;
	std::map<std::string, Var> vars;
	
	//String stuff
	int str_count = 0;
	std::map<std::string, std::string> dec_strings;
	
	//Float stuff
	int flt_count = 0;
	std::map<float, std::string> dec_flt;
	
	//Label stuff
	int lbl_count = 0;
	std::stack<std::string> labels;
	std::stack<std::string> end_lbls;
};
