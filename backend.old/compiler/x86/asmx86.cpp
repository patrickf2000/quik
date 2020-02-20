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
			case AstType::StructMod:
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
			
			case AstType::StructDec: build_struct_dec(node); break;
			case AstType::Struct: build_struct_var(node); break;
			
			case AstType::Loop:
			case AstType::ForEach:
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
				//TODO: Is there a way we can clean this up?
				if (node->type == AstType::Loop || node->type == AstType::ForEach) {
					stack_pos += 4;
					std::string dest = "[" + get_reg("bp");
					dest += "-" + std::to_string(stack_pos) + "]";
					
					sec_text.push_back("mov DWORD PTR " + dest + ", 0");
					
					if (node->type == AstType::ForEach) {
						AstForEach *fe = static_cast<AstForEach *>(node);
					
						//Build the variable used to hold the for-each value
						Var v;
						v.name = fe->i_var;
						v.stack_pos = stack_pos;
						v.is_array = false;
						v.is_param = false;
						v.type = vars[fe->r_var].type;
						
						if (v.type == DataType::Int128 || v.type == DataType::Int256)
							v.type = DataType::Int;
							
						vars[fe->i_var] = v;
						
						//Build the internal index counter
						stack_pos += 4;
						dest = "[" + get_reg("bp");
						dest += "-" + std::to_string(stack_pos) + "]";
						
						sec_text.push_back("mov DWORD PTR " + dest + ", 0");
						
						fe->i_var_in = dest;
						node = fe;
					} else {
						AstLoop *lp = static_cast<AstLoop *>(node);
						lp->i_var = dest;
						node = lp;
					}
				}
				
				sec_text.push_back("jmp " + cmp_lbl);
				sec_text.push_back(top_lbl + ":");
				
				//If we have a foreach, we have to handle the top
				if (node->type == AstType::ForEach)
					build_foreach_top(node);
				
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
	
	writer << ".intel_syntax noprefix" << std::endl;
	writer << ".data" << std::endl;
	
	for (auto l : sec_data) {
		writer << "\t" << l << std::endl;
	}
	
	writer << std::endl;
	writer << ".bss" << std::endl;
	
	for (auto l : sec_bss) {
		writer << "\t" << l << std::endl;
	}
	
	writer << std::endl;
	writer << ".text" << std::endl;
	
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
	std::string ld_line = "ld ";
	
	if (x64) {
		ld_line += "/usr/lib/x86_64-linux-gnu/crti.o ";
		ld_line += "/usr/lib/x86_64-linux-gnu/crtn.o ";
		ld_line += "/usr/lib/x86_64-linux-gnu/crt1.o -lc ";
		ld_line += "-lqkstdlib ";
	} else {
		ld_line += "/usr/lib32/crti.o ";
		ld_line += "/usr/lib32/crtn.o ";
		ld_line += "/usr/lib32/crt1.o -lc ";
		ld_line += "-lqkstdlib32 ";
		ld_line += "-melf_i386 ";
	}

	for (auto l : config.libs) {
		ld_line += "-l" + l + " ";
	}
	
	/*if (config.build_lib) {
		ld_line += "-shared -fPIC ";
	}*/

	for (auto p : asm_files) {
		auto o_out = get_basename(p);
		o_out = "/tmp/" + o_out + ".o";
		ld_line += o_out + " ";
		
		std::string as_line = "as -g ";
		
		if (!x64)
			as_line += " --32 ";
		
		as_line += p + " -o " + o_out;
		
		system(as_line.c_str());
	}
	
	if (x64)
		ld_line += "-dynamic-linker /lib64/ld-linux-x86-64.so.2 ";
	else
		ld_line += "-dynamic-linker /lib32/ld-linux.so.2 ";
		
	ld_line += "-o ";
	ld_line += config.out_name; 

	system(ld_line.c_str());
}

