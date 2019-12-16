#include <iostream>
#include <fstream>
#include <types.hh>
#include <cstdlib>

#include "asmx86.hh"
#include "utils.hh"

//Set things up
Asm_x86::Asm_x86() {
	extern_data.push_back("global main");
}

//Iterate through the tree and assemble
void Asm_x86::assemble(std::string p, AstNode *top) {
	if (p != "") {
		std::string base = get_basename(p);
		path = "/tmp/" + base + ".asm";
		asm_files.push_back(base);
	}

	for (auto node : top->children) {
		if (node->type == AstType::Scope) {
			current_scope = dynamic_cast<AstScope *>(node);
			assemble("", node);
		} else if (node->type == AstType::FuncDec) {
			build_function(node);
			assemble("", node);
		} else if (node->type == AstType::ExternFunc) {
			build_extern_func(node);
		} else if (node->type == AstType::VarDec) {
			build_var_dec(node);
		} else if (node->type == AstType::VarAssign) {
			build_var_assign(node);
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
	
	//Build the arguments
	int arg_access = 4;
	
	for (auto v : fd->args) {
		//Pop the arguments into the variables
		std::string mov_ln = "mov eax, [esp+";
		mov_ln += std::to_string(arg_access) + "]";
		
		arg_access += 4;
		sec_text.push_back(mov_ln);
		
		/*if (v.type == DataType::Str)
			sec_text.push_back("mov " + v.name + ", eax");
		else*/
			sec_text.push_back("mov [" + v.name + "], eax");
	
		//Declare the function arguments in assembly
		AstVarDec *vd = new AstVarDec(v.name);
		vd->set_type(v.type);
		
		if (v.type == DataType::Str) {
			vd->children.push_back(new AstString);
		} else if (v.type == DataType::Bool) {
			//TODO: Replace
			vd->children.push_back(new AstNode);
		} else {
			vd->children.push_back(new AstInt(0));
		}
		
		build_var_dec(vd);
		sec_text.push_back("");
	}
}

//Assembles an external function
void Asm_x86::build_extern_func(AstNode *node) {
	AstExternFunc *fd = dynamic_cast<AstExternFunc *>(node);
	std::string ln = "extern " + fd->get_name();
	
	extern_data.push_back(ln);
}

//Assembles a function call
void Asm_x86::build_func_call(AstFuncCall *fc) {
	for (int i = fc->children.size()-1; i>=0; i--) {
		auto node = fc->children.at(i);
	
		switch (node->type) {
			//An ID-> Means we have a variable
			case AstType::Id: {
				AstID *id = dynamic_cast<AstID *>(node);
				Var v = current_scope->vars[id->get_name()];
				std::string ln = "push dword ";
			
				switch (v.type) {
					case DataType::Byte:
					case DataType::Char:
					case DataType::Short:
					case DataType::Bool:
					case DataType::Int: ln += "[" + v.name + "]"; break;
					case DataType::Long:
					case DataType::Float: {
							ln += "[" + v.name + "+4]";
							ln += "[" + v.name + "]";
						} break;
					case DataType::Str: ln += v.name; break;
				}
				
				sec_text.push_back(ln);
			} break;
			
			//An Int
			case AstType::Int: {
				AstInt *no = dynamic_cast<AstInt *>(node);
				sec_text.push_back("push " + std::to_string(no->get_val()));
			} break;
			
			//A string
			case AstType::Str: {
				AstString *str = dynamic_cast<AstString *>(node);
				
				std::string name = "STR_" + std::to_string(str_index);
				sec_data.push_back(name + " db \"" + str->get_val() + "\",0");
				++str_index;
				
				sec_text.push_back("push dword " + name);
			} break;
		}
	}

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
		//Align the call stack
		if (!in_main)
			sec_text.push_back("push eax");
		
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
			} break;
		}
		
		//Finish aligning the call stack
		if (!in_main)
			sec_text.push_back("add esp, 12");
		
		sec_text.push_back("");
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
	
	if (first->type == AstType::Id || vd->children.size() > 1) {
		ln += "0";
		build_var_assign(node);
	} else {
		if (first->type == AstType::Int) {
			AstInt *i = dynamic_cast<AstInt *>(first);
			ln += std::to_string(i->get_val());
		} else if (first->type == AstType::Str) {
			AstString *i = dynamic_cast<AstString *>(first);
			ln += "\"" + i->get_val() + "\",0";
		}
	}
	
	sec_data.push_back(ln);
}

//Builds a variable assignment
void Asm_x86::build_var_assign(AstNode *node) {
	AstAttrNode *va = dynamic_cast<AstAttrNode *>(node);
	auto child = node->children.at(0);
		
	switch (child->type) {
		//Integers
		case AstType::Int: {
			AstInt *i = dynamic_cast<AstInt *>(child);
			auto val = std::to_string(i->get_val());
		
			sec_text.push_back("mov eax, " + val);
		} break;
		
		//Other variables
		case AstType::Id: {
			AstID *id = dynamic_cast<AstID *>(child);
			sec_text.push_back("mov eax, [" + id->get_name() + "]");
		} break;
	}
	
	//Iterate through all the children
	for (int i = 1; i<va->children.size(); i+=2) {
		auto current = va->children.at(i);
		auto next = va->children.at(i+1);
		std::string ln = "";
		
		bool is_mod = false;
		bool is_div = false;
		
		switch (current->type) {
			case AstType::Add: ln += "add eax, "; break;
			case AstType::Sub: ln += "sub eax, "; break;
			case AstType::Mul: ln += "imul eax, "; break;
			case AstType::Mod: is_mod = true;
			case AstType::Div: is_div = true; break;
		}
		
		if (is_div) {
			ln = "mov ebx, ";
		}
		
		switch (next->type) {
			case AstType::Int: {
				AstInt *i = dynamic_cast<AstInt *>(next);
				ln += std::to_string(i->get_val());
			} break;
			
			case AstType::Id: {
				AstID *id = dynamic_cast<AstID *>(next);
				ln += "[" + id->get_name() + "]";
			} break;
		}
		
		sec_text.push_back(ln);
		
		if (is_div) {
			sec_text.push_back("cdq");
			sec_text.push_back("div ebx");
		}
		
		if (is_mod) {
			sec_text.push_back("mov eax, edx");
		}
	}
	
	sec_text.push_back("mov [" + va->get_name() + "], eax");
	sec_text.push_back("");
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
	system(std::string("nasm -g -f elf32 " + path + " -o /tmp/out.o").c_str());
	system("gcc -g -m32 /tmp/out.o -o out");
}

