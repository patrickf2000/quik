#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>

#include "armv7.hh"
#include "../utils.hh"

//The constructor
Asm_Armv7::Asm_Armv7() {
	config.out_name = "out";
}

Asm_Armv7::Asm_Armv7(Config cf) {
	config = cf;
}

//Our recursive assembly function
void Asm_Armv7::assemble(std::string f_path, AstNode *top) {
	if (f_path != "")
		path = f_path;

	for (auto node : top->children) {
		switch (node->type) {
			case AstType::Scope: assemble("", node); break;
			
			case AstType::FuncDec: {
				build_func_dec(node);
				assemble("", node);
			} break;
			
			case AstType::FuncCall: build_func_call(node); break;
			case AstType::ExternFunc: build_extern_func(node); break;
			
			case AstType::VarDec: build_var_dec(node); break;
			case AstType::VarAssign: build_var_assign(node); break;
			
			case AstType::Return: build_ret(node); break;
		}
	}
}

//Writes everything out into a file
void Asm_Armv7::write() {
	std::string as_path = "/tmp/" + get_basename(path) + ".asm";
	std::ofstream writer(as_path);

	//Write the data section
	writer << ".data" << std::endl;
	
	for (auto ln : sec_data) {
		writer << "\t" << ln << std::endl;
	}
	
	//Write the extern section
	writer << std::endl;
	writer << ".text" << std::endl;
	writer << ".global main" << std::endl;
	
	for (auto ln : extern_data)
		writer << ln << std::endl;
	
	//Write the text section
	writer << std::endl;
	
	for (auto ln : sec_text) {
		if (ln[ln.length()-1] == ':')
			writer << ln << std::endl;
		else
			writer << "\t" << ln << std::endl;
	}
	
	//Write the bottom data section
	writer << std::endl;
	
	for (auto ln : btm_data) {
		writer << ln << std::endl;
	}
	
	//Close everything up...
	writer.close();
}

//Issues the proper command to build the program
//TODO: This needs a lot of work
void Asm_Armv7::build() {
	std::string as_path = "/tmp/" + get_basename(path) + ".asm";
	std::string o_path = "/tmp/" + get_basename(path) + ".o";
	std::string out_path = get_path(path) + config.out_name;
	
	std::string as_cmd = "as " + as_path + " -o " + o_path;
	std::string ln_cmd = "gcc " + o_path + " -o " + out_path + " -lqkstdlib";

	system(as_cmd.c_str());
	system(ln_cmd.c_str());
}

