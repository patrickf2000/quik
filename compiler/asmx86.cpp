#include <iostream>
#include <fstream>
#include <types.hh>
#include <cstdlib>

#include "asmx86.hh"

//Set things up
Asm_x86::Asm_x86(std::string p) {
	path = p;
	
	extern_data.push_back("global main");
}

//Iterate through the tree and assemble
void Asm_x86::assemble(AstNode *top) {
	for (auto node : top->children) {
		if (node->type == AstType::Scope) {
			current_scope = dynamic_cast<AstScope *>(node);
			assemble(node);
		} else if (node->type == AstType::FuncDec) {
			build_function(node);
			assemble(node);
		} else if (node->type == AstType::VarDec) {
			build_var_dec(node);
		} else if (node->type == AstType::FuncCall) {
			AstFuncCall *fc = dynamic_cast<AstFuncCall *>(node);
		
			if (fc->get_name() == "println") {
				build_println(fc);
			} else {
				build_func_call(fc);
			}
		} else if (node->type == AstType::Return) {
			build_ret(node);
		}
	}
}

//Assembles a function
void Asm_x86::build_function(AstNode *node) {
	AstFuncDec *fd = dynamic_cast<AstFuncDec *>(node);
	
	std::string ln = fd->get_name();
	if (ln == "main") {
		in_main = true;
	} else {
		in_main = false;
	}
	
	ln += ":";
	sec_text.push_back(ln);
}

//Assembles a function call
//TODO: This needs a lot of love
void Asm_x86::build_func_call(AstFuncCall *fc) {
	sec_text.push_back("call " + fc->get_name());
	sec_text.push_back("");	
}

//Assembles a println call
void Asm_x86::build_println(AstFuncCall *fc) {
	//We wrap this as a conditional in case this built-in is never called
	if (!use_printf) {
		//Push the info we need back
		extern_data.push_back("extern printf");
		
		//TODO: Add some conditional so we don't push everything
		sec_data.push_back("int_fmt db \"%d\",10,0");
		sec_data.push_back("str_fmt db \"%s\",10,0");
		sec_data.push_back("flt_fmt db \"%f\",10,0");
		
		use_printf = true;
	}
	
	//Add the code
	for (auto node : fc->children) {
		switch (node->type) {
			//Hard-coded string
			case AstType::Str: {
				AstString *str = dynamic_cast<AstString *>(node);
			
				std::string name = "STR_" + std::to_string(str_index);
				sec_data.push_back(name + " db \"" + str->get_val() + "\",0");
				++str_index;
				
				sec_text.push_back("push dword " + name);
				sec_text.push_back("push dword str_fmt");
				sec_text.push_back("call printf");
				sec_text.push_back("");
			} break;
			
			//Variable
			case AstType::Id: {
				AstID *id = dynamic_cast<AstID *>(node);
				Var v = current_scope->vars[id->get_name()];
				
				std::string name = v.name;
				std::string p2 = "";
				std::string fmt = "";
				
				switch (v.type) {
					case DataType::Byte:
					case DataType::Char:
					case DataType::Short:
					case DataType::Bool:
					case DataType::Int: {
							name = "[" + v.name + "]";
							fmt = "int_fmt"; 
						} break;
					case DataType::Long:
					case DataType::Float: {
						name = "[" + v.name + "+4]";
						p2 = "[" + v.name + "]";
						fmt = "str_fmt";
					} break;
					case DataType::Str:	fmt = "str_fmt";		
				}
				
				sec_text.push_back("push dword " + name);
				if (p2 != "")
					sec_text.push_back("push dword " + p2);
					
				sec_text.push_back("push dword " + fmt);
				sec_text.push_back("call printf");
				sec_text.push_back("");
			} break;
		}
	}
}

//Builds the return statements
void Asm_x86::build_ret(AstNode *node) {
	if (in_main) {
		sec_text.push_back("mov eax, 1");
		sec_text.push_back("mov ebx, 0");
		sec_text.push_back("int 0x80");
		sec_text.push_back("");
	} else {
		sec_text.push_back("ret");
	}
	
	sec_text.push_back("");
}

//Assembles a variable declaration
void Asm_x86::build_var_dec(AstNode *node) {
	AstVarDec *vd = dynamic_cast<AstVarDec *>(node);
	std::string ln = vd->get_name() + " ";
	
	switch (vd->get_type()) {
		case DataType::Byte:
		case DataType::Char:
		case DataType::Bool:
		case DataType::Str: ln += "db "; break;
		case DataType::Short: ln += "dw "; break;
		case DataType::Int: ln += "dd "; break;
		case DataType::Long:
		case DataType::Float: ln += "dq "; break;
	}
	
	auto first = vd->children.at(0);
	
	if (first->type == AstType::Id) {
		AstID *id = dynamic_cast<AstID *>(first);
		ln += "0";
		
		sec_text.push_back("mov eax, [" + id->get_name() + "]");
		sec_text.push_back("mov [" + vd->get_name() + "], eax");
		sec_text.push_back("");
	} else {
		if (first->type == AstType::Int) {
			AstInt *i = dynamic_cast<AstInt *>(first);
			ln += std::to_string(i->get_val());
		}
	}
	
	sec_data.push_back(ln);
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
}

//Invoke system commands to build the final executable
void Asm_x86::build() {
	//TODO: Fix this
	system(std::string("nasm -f elf32 " + path + " -o /tmp/out.o").c_str());
	system("gcc -m32 /tmp/out.o -o out");
}

