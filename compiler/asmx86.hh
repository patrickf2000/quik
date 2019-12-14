#pragma once

#include <string>
#include <vector>

#include <ast.hh>

class Asm_x86 {
public:
	Asm_x86(std::string p);
	void assemble(AstNode *top);
	void build_function(AstNode *node);
	void build_println(AstFuncCall *fc);
	void build_var_dec(AstNode *node);
	void write();
	void build();
private:
	std::string path = "";
	std::vector<std::string> sec_data;
	std::vector<std::string> sec_text;
	std::vector<std::string> extern_data;
	bool in_main = false;
	int str_index = 1;
};
