#include <fstream>
#include <iostream>
#include <string>

#include "armv7.hh"

//The constructor
Asm_Armv7::Asm_Armv7(std::string f_path) {
	path = f_path;
}

//Our recursive assembly function
void Asm_Armv7::assemble(AstNode *top) {
	for (auto node : top->children) {
		switch (node->type) {
			case AstType::Scope: assemble(node); break;
			
			case AstType::FuncDec: {
				build_func_dec(node);
				assemble(node);
			} break;
			
			case AstType::Return: build_ret(node); break;
		}
	}
}

//Writes everything out into a file
void Asm_Armv7::write() {
	std::ofstream writer(path);

	//Write the data section
	writer << ".data" << std::endl;
	
	for (auto ln : sec_data) {
		writer << "\t" << ln << std::endl;
	}
	
	//Write the text section
	writer << std::endl;
	writer << ".text" << std::endl;
	writer << ".global main" << std::endl;
	writer << std::endl;
	
	for (auto ln : sec_text) {
		if (ln[ln.length()-1] == ':')
			writer << ln << std::endl;
		else
			writer << "\t" << ln << std::endl;
	}
	
	//Close everything up...
	writer.close();
}
