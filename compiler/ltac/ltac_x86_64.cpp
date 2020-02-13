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
			//Extern directives
			case LTAC::Extern: {
				writer << ".extern " << node->val << std::endl;
			} break;
		
			//Functions and labels
			case LTAC::Func: writer << std::endl;
			case LTAC::Label: {
				writer << node->val << ":" << std::endl;
			} break;
			
			//Function setup
			case LTAC::Setup: {
				writer << "\tpush rbp" << std::endl;
				writer << "\tmov rbp, rsp" << std::endl << std::endl;
			} break;
			
			//Function end
			case LTAC::Leave: {
				writer << "\tleave" << std::endl;
			} break;
			
			//Return
			case LTAC::Ret: {
				writer << "\tret" << std::endl << std::endl;
			} break;
			
			//Move
			case LTAC::Mov: {
				//Perform some translations
				auto left = node->args[0];
				auto right = node->args[1];
				
				auto arg_left = build_operand(left);
				auto arg_right = build_operand(right);
				
				if (arg_left == "" || arg_right == "")
					break;
				
				//Write the code
				writer << "\tmov ";
				
				if (right->type == LTAC::Int) {
					writer << "DWORD PTR ";
				}
				
				writer << arg_left << ", " << arg_right;
				writer << std::endl;
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

//Builds the operand of certain statements
//Examples: registers, memory locations, raw values, etc
std::string LTAC_Generator::build_operand(LtacNode *node) {
	std::string ln = "";
	
	switch (node->type) {
		case LTAC::Mem: {
			LtacMem *mem = static_cast<LtacMem *>(node);
			int loco = stack_base + mem->index;
			ln = "[rbp-" + std::to_string(loco);
			
			if (mem->scale > 0)
				ln += "*" + std::to_string(mem->scale);
				
			ln += "]";
		} break;
		
		case LTAC::Int: {
			LtacInt *i = static_cast<LtacInt *>(node);
			ln = std::to_string(i->get_val());
		} break;
	}
	
	return ln;
}

