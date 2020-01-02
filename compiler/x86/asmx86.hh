#pragma once

#include <string>
#include <vector>
#include <stack>
#include <iostream>

#include <ast.hh>

#include "../types.hh"

class Asm_x86 {
public:
	explicit Asm_x86();
	explicit Asm_x86(Config c);
	void assemble(std::string p, AstNode *top);
	void build_function(AstNode *node);
	void build_extern_func(AstNode *node);
	void build_func_call(AstFuncCall *fc);
	void build_ret(AstNode *node);
	void build_var_dec(AstNode *node);
	void build_var_assign(AstNode *node);
	void build_flt_assign(AstNode *node);
	void build_arr_dec(AstNode *node);
	void build_arr_access(AstNode *node);
	void build_cond(AstNode *node);
	void build_while(AstNode *node);
	void write();
	void build();
	void build_link();
protected:
	std::string type2asm(AstNode *node);
	void type2flt(AstNode *node);
	std::string build_string(AstNode *node);
private:
	std::string path = "";
	std::vector<std::string> asm_files;
	std::vector<std::string> sec_data;
	std::vector<std::string> sec_bss;
	std::vector<std::string> sec_text;
	std::vector<std::string> extern_data;
	
	//The config structure
	Config config;
	
	//Variable stuff
	std::map<std::string, Var> vars;
	int stack_pos = 0;
	
	//Control variables
	bool in_main = false;
	int str_index = 1;
	
	//Needed for conditionals and loops
	int lbl_index = 1;
	std::stack<std::string> labels;
	std::stack<std::string> top_lbls;
	
	//Scope data
	AstScope *current_scope;
};
