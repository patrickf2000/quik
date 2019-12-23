//asmx86
//Contains the control functions for the x86 backend
#include <iostream>
#include <fstream>
#include <types.hh>
#include <cstdlib>

#include "asmx86.hh"
#include "../utils.hh"

//The default constructor
Asm_x86::Asm_x86() {
	config.out_name = "out";
}

//The config constructor
Asm_x86::Asm_x86(Config c) {
	config = c;
}

//Iterate through the tree and assemble
void Asm_x86::assemble(std::string p, AstNode *top) {
	if (p != "") {
		std::string base = get_basename(p);
		path = "/tmp/" + base + ".asm";
		asm_files.push_back(path);
	}

	for (auto node : top->children) {
		switch (node->type) {
			case AstType::Scope: {
				current_scope = dynamic_cast<AstScope *>(node);
				assemble("", node);
			} break;
			
			case AstType::FuncDec: {
				build_function(node);
				assemble("", node);
			} break;
			
			case AstType::ExternFunc: build_extern_func(node); break;
			case AstType::VarDec: build_var_dec(node); break;
			case AstType::VarAssign: 
			case AstType::ArrayAssign: build_var_assign(node); break;
			case AstType::ArrayDec: build_arr_dec(node); break;
			
			case AstType::FuncCall: {
				AstFuncCall *fc = dynamic_cast<AstFuncCall *>(node);
			
				if (fc->get_name() == "println") {
					build_println(fc);
				} else {
					build_func_call(fc);
				}
			} break;
			
			case AstType::If: {
				if (node->type == AstType::If) {
					std::string lbl = "L" + std::to_string(lbl_index);
					top_lbls.push(lbl);
					++lbl_index;	
				}
			}
			case AstType::Elif:
			case AstType::Else: {
				build_cond(node);
				assemble("", node);
				
				sec_text.push_back("jmp " + top_lbls.top());
				
				sec_text.push_back(labels.top() + ":");
				labels.pop();
			} break;
			
			case AstType::EndIf: {
				sec_text.push_back(top_lbls.top() + ":");
				top_lbls.pop();
			} break;
			
			case AstType::While: {
				std::string top_lbl = "L" + std::to_string(lbl_index);
				++lbl_index;
				labels.push(top_lbl);
				
				std::string cmp_lbl = "L" + std::to_string(lbl_index);
				++lbl_index;
				labels.push(cmp_lbl);
				
				sec_text.push_back("jmp " + cmp_lbl);
				sec_text.push_back(top_lbl + ":");
				
				assemble("", node);
				build_while(node);
			} break;
			
			case AstType::Return: build_ret(node); break;
		}
	}
}

//Write out the final product
void Asm_x86::write() {
	std::ofstream writer(path);
	
	writer << "section .data" << std::endl;
	
	for (auto l : sec_data) {
		writer << "\t" << l << std::endl;
	}
	
	writer << std::endl;
	writer << "section .text" << std::endl;
	
	for (auto l : extern_data) {
		writer << "\t" << l << std::endl;
	}
	
	writer << std::endl;
	
	for (auto l : sec_text) {
		if (l[l.length()-1] != ':') {
			writer << "\t";
		}
		writer << l << std::endl;
	}
	
	//Clear the lists
	sec_data.clear();
	sec_text.clear();
	extern_data.clear();
	use_printf = false;
}

//Invoke system commands to build the final executable
void Asm_x86::build() {
	std::string gcc_line = "gcc -g -m32 ";

	//TODO: Fix this
	for (auto p : asm_files) {
		auto o_out = get_basename(p);
		o_out = "/tmp/" + o_out + ".o";
		gcc_line += o_out + " ";
		
		system(std::string("nasm -g -f elf32 " + p + " -o " + o_out).c_str());
	}
	
	gcc_line += " -o ";
	gcc_line += config.out_name; 
	gcc_line += " -L./ -lqkstdlib";
	system(gcc_line.c_str());
}

