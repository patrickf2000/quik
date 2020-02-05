#pragma once

#include <string>
#include <vector>

#include <ast.hh>

class Asm_Armv7 {
public:
	explicit Asm_Armv7(std::string f_path);
	void assemble(AstNode *node);
	void write();
private:
	std::string path = "";
	
	//Holds various section parts
	std::vector<std::string> sec_data;
	std::vector<std::string> sec_text;
};
