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
	config.build_lib = false;
	config.obj_only = false;
	config.arch = "x86_64";
	
	init_registers();
}

//The config constructor
Asm_x86::Asm_x86(Config c) {
	config = c;
	
	if (config.arch == "x86_64")
		x64 = true;
	else
		x64 = false;
		
	init_registers();
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
				build_func_call(fc);
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
			
			case AstType::Loop:
			case AstType::While: {
				std::string top_lbl = "L" + std::to_string(lbl_index);
				++lbl_index;
				labels.push(top_lbl);
				
				std::string cmp_lbl = "L" + std::to_string(lbl_index);
				++lbl_index;
				labels.push(cmp_lbl);
				
				//This is the only chunk of code specific to the loop
				//We have to zero-out the segment of memory we use to hold the
				// index in order to safely use it
				if (node->type == AstType::Loop) {
					stack_pos += 4;
					std::string dest = "[" + get_reg("bp");
					dest += "-" + std::to_string(stack_pos) + "]";
					
					AstLoop *lp = static_cast<AstLoop *>(node);
					lp->i_var = dest;
					node = lp;
					
					sec_text.push_back("mov dword " + dest + ", 0");
				}
				
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
	writer << "section .bss" << std::endl;
	
	for (auto l : sec_bss) {
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
}

//Checks to see how we wish to build
void Asm_x86::build() {
	if (!config.obj_only) {
		build_link();
		return;
	}
	
	for (auto p : asm_files) {
		auto o_out = get_basename(p);
		o_out = "./" + o_out + ".o";
		
		std::string nasm_line = "nasm -g -f";
		
		if (x64)
			nasm_line += " elf64 ";
		else
			nasm_line += " elf32 ";
			
		nasm_line += p + " -o " + o_out;
		
		system(nasm_line.c_str());
	}
}

//Invoke system commands to build the final executable
void Asm_x86::build_link() {
	std::string gcc_line = "gcc -g ";
	
	if (x64)
		gcc_line += "-no-pie ";
	else
		gcc_line += "-m32 ";
	
	if (config.build_lib) {
		gcc_line += "-shared -fPIC ";
	}

	for (auto p : asm_files) {
		auto o_out = get_basename(p);
		o_out = "/tmp/" + o_out + ".o";
		gcc_line += o_out + " ";
		
		std::string nasm_line = "nasm -g -f";
		
		if (x64)
			nasm_line += " elf64 ";
		else
			nasm_line += " elf32 ";
		
		nasm_line += p + " -o " + o_out;
		
		system(nasm_line.c_str());
	}
	
	gcc_line += " -o ";
	gcc_line += config.out_name; 
	
	if (x64)
		gcc_line += " -L./ -lqkstdlib -L./ ";
	else
		gcc_line += " -L./ -lqkstdlib32 -L./ ";
	
	for (auto l : config.libs) {
		gcc_line += "-l" + l + " ";
	}
	
	system(gcc_line.c_str());
}


