#include <iostream>
#include <fstream>
#include <cstdlib>

#include <ltac.hh>

#include "ltac_gen.hh"

//Generates code for the x86-64 platform
void LTAC_Generator::build_x86_64(LtacFile *file) {
	output = file->path;
	std::ofstream writer(output + ".asm");
	
	//Write the data section
	writer << ".intel_syntax noprefix" << std::endl;
	writer << ".data" << std::endl;
	
	for (auto str : file->str_pool) {
		writer << "\t" << str->get_name() << ": .string \""
			<< str->get_val() << "\"" << std::endl;
	}
	
	//Write the code section
	writer << std::endl;
	writer << ".text" << std::endl;
	writer << ".global main" << std::endl;
	writer << std::endl;
	
	for (auto node : file->children) {
		switch (node->type) {
			case LTAC::Extern: {
				writer << ".extern " << node->val << std::endl;
			} break;
		
			case LTAC::Func: {
				writer << std::endl;
				writer << node->val << ":" << std::endl;
			} break;
		}
	}
	
	//Close the writer
	writer.close();
}

//Compile
void LTAC_Generator::compile_x86_64(std::string bin) {
	std::string as_ln = "as " + output + ".asm -o " + output + ".o";
	std::string link_ln = "gcc " + output + ".o -no-pie -o " + bin;
	
	system(as_ln.c_str());
	system(link_ln.c_str());
}
