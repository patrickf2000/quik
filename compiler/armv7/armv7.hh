#pragma once

#include <string>
#include <vector>
#include <map>

#include <ast.hh>

class Asm_Armv7 {
public:
	explicit Asm_Armv7(std::string f_path);
	void assemble(AstNode *top);
	void write();
	void build();
	
	//Function stuff
	void build_func_dec(AstNode *node);
	void build_func_call(AstNode *node);
	void build_extern_func(AstNode *node);
	void build_ret(AstNode *node);
	
	//Variable stuff
	void build_var_dec(AstNode *node);
	void build_var_assign(AstNode *node);
	
protected:
	//Utility stuff
	std::string build_string(AstNode *node);
private:
	std::string path = "";
	
	//Holds various section parts
	std::vector<std::string> sec_data;
	std::vector<std::string> sec_text;
	std::vector<std::string> btm_data;
	std::vector<std::string> extern_data;
	
	//Control variables
	int str_lbl = 0;
	
	//Variable stuff
	std::map<std::string, Var> vars;
	int stack_pos = 8;
};
