#include <iostream>
#include <fstream>
#include <types.hh>
#include <cstdlib>

#include "asmx86.hh"
#include "utils.hh"

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
			case AstType::VarAssign: build_var_assign(node); break;
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

//Assembles a function
void Asm_x86::build_function(AstNode *node) {
	AstFuncDec *fd = dynamic_cast<AstFuncDec *>(node);
	
	std::string ln = fd->get_name();
	extern_data.push_back("global " + ln);
	
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
		if (v.type == DataType::Float) {
			sec_text.push_back("fstp qword [" + v.name + "]");
		} else {
			//Pop the arguments into the variables
			std::string mov_ln = "mov eax, [esp+";
			mov_ln += std::to_string(arg_access) + "]";
			
			arg_access += 4;
			sec_text.push_back(mov_ln);

			sec_text.push_back("mov [" + v.name + "], eax");
		}
	
		//Declare the function arguments in assembly
		AstVarDec *vd = new AstVarDec(v.name);
		vd->set_type(v.type);
		
		if (v.type == DataType::Str) {
			vd->children.push_back(new AstString);
		} else if (v.type == DataType::Bool) {
			vd->children.push_back(new AstBool(false));
		} else if (v.type == DataType::Float) {
			vd->children.push_back(new AstFloat(0.0));
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
				std::string ln = "";
			
				switch (v.type) {
					case DataType::Byte:
					case DataType::Char:
					case DataType::Short:
					case DataType::Bool:
					case DataType::Int: {
						ln = "push dword ";
						ln += "[" + v.name + "]"; 
					} break;
					case DataType::Long:
					case DataType::Float: {
							ln = "fld qword [" + v.name + "]";
						} break;
					case DataType::Str: ln = "push dword " + v.name; break;
				}
				
				sec_text.push_back(ln);
			} break;
			
			//An Int
			case AstType::Int: {
				AstInt *no = dynamic_cast<AstInt *>(node);
				sec_text.push_back("push " + std::to_string(no->get_val()));
			} break;
			
			//A float
			case AstType::Float: type2flt(node); break;
			
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

		sec_data.push_back("int_fmt db \"%d\",10,0");
		sec_data.push_back("str_fmt db \"%s\",10,0");
		sec_data.push_back("flt_fmt db \"%f\",10,0");
		
		use_printf = true;
	}
	
	//Add the code
	for (auto node : fc->children) {
		bool was_flt = false;
	
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
						fmt = "flt_fmt";
						was_flt = true;
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
		if (!in_main) {
			if (was_flt)
				sec_text.push_back("add esp, 16");
			else
				sec_text.push_back("add esp, 12");
		}
		
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
		return;
	}
	
	if (node->children.size() == 1) {
		auto child = node->children.at(0);
		if (child->type == AstType::Float) {
			type2flt(child);
		} else if (child->type == AstType::Id) {
			AstID *id = dynamic_cast<AstID *>(child);
			Var v = current_scope->vars[id->get_name()];
			
			if (v.type == DataType::Float)
				type2flt(child);
			else
				sec_text.push_back("mov eax, " + type2asm(child));
		} else {
			sec_text.push_back("mov eax, " + type2asm(child));
		}
	}
	
	sec_text.push_back("ret");
	sec_text.push_back("");
}

//Assembles a variable declaration
void Asm_x86::build_var_dec(AstNode *node) {
	AstVarDec *vd = dynamic_cast<AstVarDec *>(node);
	std::string ln = vd->get_name() + " ";
	
	switch (vd->get_type()) {
		case DataType::Byte:
		case DataType::Char:
		case DataType::Str: ln += "db "; break;
		case DataType::Short: ln += "dw "; break;
		case DataType::Bool:
		case DataType::Int: ln += "dd "; break;
		case DataType::Long:
		case DataType::Float: ln += "dq "; break;
	}
	
	auto first = vd->children.at(0);
	
	if (first->type == AstType::Id || first->type == AstType::FuncCall 
			|| first->type == AstType::ArrayAccess
			|| vd->children.size() > 1) {
		ln += "0";
		build_var_assign(node);
	} else {
		if (first->type == AstType::Int) {
			AstInt *i = dynamic_cast<AstInt *>(first);
			ln += std::to_string(i->get_val());
		} else if (first->type == AstType::Str) {
			AstString *i = dynamic_cast<AstString *>(first);
			ln += "\"" + i->get_val() + "\",0";
		} else if (first->type == AstType::Float) {
			AstFloat *i = dynamic_cast<AstFloat *>(first);
			ln += std::to_string(i->get_val());
		} else if (first->type == AstType::Bool) {
			AstBool *i = dynamic_cast<AstBool *>(first);
			if (i->get_val())
				ln += "1";
			else
				ln += "0";
		}
	}
	
	sec_data.push_back(ln);
}

//Builds a variable assignment
void Asm_x86::build_var_assign(AstNode *node) {
	AstVarDec *va = dynamic_cast<AstVarDec *>(node);
	if (va->get_type() == DataType::Float) {
		build_flt_assign(node);
		return;
	}
	
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
		
		//Function calls
		case AstType::FuncCall: {
			AstFuncCall *fc = dynamic_cast<AstFuncCall *>(child);
			build_func_call(fc);
		} break;
		
		//Array access
		case AstType::ArrayAccess: {
			build_arr_access(child);
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
		
		if (next->type == AstType::FuncCall) {
			sec_text.push_back("mov [" + va->get_name() + "], eax");
			AstFuncCall *fc = dynamic_cast<AstFuncCall *>(next);
			build_func_call(fc);
			ln += "[" + va->get_name() + "]";
		} else {
			ln += type2asm(next);
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

//Builds a floating-point variable assignment
void Asm_x86::build_flt_assign(AstNode *node) {
	AstAttrNode *va = dynamic_cast<AstAttrNode *>(node);
	auto child = va->children.at(0);
	
	type2flt(child);
	
	//Iterate through the children
	for (int i = 1; i<va->children.size(); i+=2) {
		auto current = va->children.at(i);
		auto next = va->children.at(i+1);
		
		type2flt(next);
		
		switch (current->type) {
			case AstType::Add: sec_text.push_back("fadd st0, st1"); break;
			case AstType::Sub: sec_text.push_back("fsubp st1, st0"); break;
			case AstType::Mul: sec_text.push_back("fmulp st1, st0"); break;
			case AstType::Div: sec_text.push_back("fdivp st1, st0"); break;
		}
	}
	
	sec_text.push_back("fstp qword [" + va->get_name() + "]");
	sec_text.push_back("");
}

//Builds an array declaration
void Asm_x86::build_arr_dec(AstNode *node) {
	AstArrayDec *ard = dynamic_cast<AstArrayDec *>(node);
	std::string ln = ard->get_name() + " ";
	
	switch (ard->get_type()) {
		case DataType::Byte:
		case DataType::Char:
		case DataType::Str: ln += "db "; break;
		case DataType::Short: ln += "dw "; break;
		case DataType::Bool:
		case DataType::Int: ln += "dd "; break;
		case DataType::Float:
		case DataType::Long: ln += "dq "; break;
	}
	
	for (int i = 0; i<ard->children.size(); i++) {
		auto child = ard->children.at(i);
		
		switch (child->type) {
			case AstType::Int: {
				AstInt *i = dynamic_cast<AstInt *>(child);
				ln += std::to_string(i->get_val());
			} break;
		}
		
		if (i+1 < ard->children.size())
			ln += ",";
	}
	
	sec_data.push_back(ln);
}

//Generates assembly for an array access statement
void Asm_x86::build_arr_access(AstNode *node) {
	AstArrayAcc *acc = dynamic_cast<AstArrayAcc *>(node);
	int index = 0;
	
	//Calculate the index
	//For ints: 4 * (element index)
	//TODO: Modify not to only use ints
	auto i_child = acc->children.at(0);
	
	switch (i_child->type) {
		case AstType::Int: {
			AstInt *i = dynamic_cast<AstInt *>(i_child);
			index = i->get_val();
			index *= 4;
		} break;
		
		case AstType::Id: {
		
		} break;
	}
	
	//Translate
	std::string ln = "mov eax, [";
	ln += acc->get_name() + "+" + std::to_string(index);
	ln += "]";
	
	sec_text.push_back(ln);
	sec_text.push_back("");
}

//Generates assembly for a conditional statement
void Asm_x86::build_cond(AstNode *node) {
	AstCond *cond = dynamic_cast<AstCond *>(node);

	//Set up the labels
	std::string lbl = "L" + std::to_string(lbl_index);
	++lbl_index;
	labels.push(lbl);
	
	if (node->type != AstType::Else) {
		//Position the lval
		std::string ln = "mov eax, " + type2asm(cond->lval);
		sec_text.push_back(ln);
		
		//Position the rval
		ln = "cmp eax, " + type2asm(cond->rval);
		sec_text.push_back(ln);
		
		//Comparison
		switch (cond->get_op()) {
			case CondOp::Equals: ln = "jne "; break;
			case CondOp::NotEquals: ln = "je "; break;
			case CondOp::Greater: ln = "jl "; break;
			case CondOp::GreaterEq: ln = "jle "; break;
			case CondOp::Less: ln = "jg "; break;
			case CondOp::LessEq: ln = "jge "; break;
		}
		
		ln += lbl;
	
		sec_text.push_back(ln);
		sec_text.push_back("");
	}
}

//Generates assembly for a while loop
void Asm_x86::build_while(AstNode *node) {
	AstCond *cond = dynamic_cast<AstCond *>(node);

	//Set up the labels
	auto cmp_lbl = labels.top();
	labels.pop();
	
	auto lbl = labels.top();
	labels.pop();
	
	sec_text.push_back(cmp_lbl + ":");
	
	//Position the lval
	std::string ln = "mov eax, " + type2asm(cond->lval);
	sec_text.push_back(ln);
	
	//Position the rval
	ln = "cmp eax, " + type2asm(cond->rval);
	sec_text.push_back(ln);
	
	//Comparison
	switch (cond->get_op()) {
		case CondOp::Equals: ln = "je "; break;
		case CondOp::NotEquals: ln = "jne "; break;
		case CondOp::Greater: ln = "jg "; break;
		case CondOp::GreaterEq: ln = "jge "; break;
		case CondOp::Less: ln = "jl "; break;
		case CondOp::LessEq: ln = "jle "; break;
	}
	
	ln += lbl;

	sec_text.push_back(ln);
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
	
	gcc_line += " -o out";
	system(gcc_line.c_str());
}

//Converts a node to an assembly type
std::string Asm_x86::type2asm(AstNode *node) {
	std::string ln = "";
	
	switch (node->type) {
		case AstType::Int: {
			AstInt *i = dynamic_cast<AstInt *>(node);
			ln = std::to_string(i->get_val());
		} break;
		
		case AstType::Id: {
			AstID *id = dynamic_cast<AstID *>(node);
			ln = "[" + id->get_name() + "]";
		} break;
		
		case AstType::Bool: {
			AstBool *bl = dynamic_cast<AstBool *>(node);
			if (bl->get_val())
				ln += "1";
			else
				ln += "0";
		} break;
	}
	
	return ln;
}

//Converts a node to floating-point Assembly type
void Asm_x86::type2flt(AstNode *node) {
	switch (node->type) {
		case AstType::Id: {
			AstID *id = dynamic_cast<AstID *>(node);
			sec_text.push_back("fld qword [" + id->get_name() + "]");
		} break;
		
		case AstType::FuncCall: {
			AstFuncCall *fc = dynamic_cast<AstFuncCall *>(node);
			build_func_call(fc);
		} break;
		
		//TODO: Is there a better way we can do this?
		case AstType::Int: {
			AstInt *f = dynamic_cast<AstInt *>(node);
			
			std::string name = "FLT_" + std::to_string(str_index);
			std::string lbl = name + " dq " + std::to_string(f->get_val());
			lbl += ".0";
			++str_index;
			
			sec_data.push_back(lbl);
			sec_text.push_back("fld qword [" + name + "]");
		} break;
		
		case AstType::Float: {
			AstFloat *f = dynamic_cast<AstFloat *>(node);
			
			std::string name = "FLT_" + std::to_string(str_index);
			std::string lbl = name + " dq " + std::to_string(f->get_val());
			++str_index;
			
			sec_data.push_back(lbl);
			sec_text.push_back("fld qword [" + name + "]");
		} break;
	}
}


