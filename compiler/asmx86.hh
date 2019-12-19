#pragma once

#include <string>
#include <vector>
#include <stack>

#include <ast.hh>

class Asm_x86 {
public:
	void assemble(std::string p, AstNode *top);
	void build_function(AstNode *node);
	void build_extern_func(AstNode *node);
	void build_func_call(AstFuncCall *fc);
	void build_println(AstFuncCall *fc);
	void build_ret(AstNode *node);
	void build_var_dec(AstNode *node);
	void build_var_assign(AstNode *node);
	void build_flt_assign(AstNode *node);
	void build_arr_dec(AstNode *node);
	void build_cond(AstNode *node);
	void build_while(AstNode *node);
	void write();
	void build();
protected:
	std::string type2asm(AstNode *node);
	void type2flt(AstNode *node);
private:
	std::string path = "";
	std::vector<std::string> asm_files;
	std::vector<std::string> sec_data;
	std::vector<std::string> sec_text;
	std::vector<std::string> extern_data;
	
	//Control variables
	bool in_main = false;
	bool use_printf = false;
	int str_index = 1;
	
	//Needed for conditionals and loops
	int lbl_index = 1;
	std::stack<std::string> labels;
	std::stack<std::string> top_lbls;
	
	//Scope data
	AstScope *current_scope;
};
