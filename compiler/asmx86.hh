#pragma once

#include <string>
#include <vector>

#include <ast.hh>

class Asm_x86 {
public:
	Asm_x86();
	void assemble(std::string p, AstNode *top);
	void build_function(AstNode *node);
	void build_extern_func(AstNode *node);
	void build_func_call(AstFuncCall *fc);
	void build_println(AstFuncCall *fc);
	void build_ret(AstNode *node);
	void build_var_dec(AstNode *node);
	void build_var_assign(AstNode *node);
	void write();
	void build();
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
	
	//Scope data
	AstScope *current_scope;
};
