#pragma once

#include <string>
#include <vector>

#include <ast.hh>

class Asm_Armv7 {
public:
	explicit Asm_Armv7(std::string f_path);
	void assemble(AstNode *top);
	void write();
	
	//Function stuff
	void build_func_dec(AstNode *node);
	void build_func_call(AstNode *node);
	void build_ret(AstNode *node);
	
protected:
	//Utility stuff
	std::string build_string(AstNode *node);
private:
	std::string path = "";
	
	//Holds various section parts
	std::vector<std::string> sec_data;
	std::vector<std::string> sec_text;
	std::vector<std::string> btm_data;
	
	//Control variables
	int str_lbl = 0;
};
