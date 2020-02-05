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
	void build_ret(AstNode *node);
private:
	std::string path = "";
	
	//Holds various section parts
	std::vector<std::string> sec_data;
	std::vector<std::string> sec_text;
};
