//x86_vars
//Contains the functions for assembling variable-specific stuff
#include "asmx86.hh"

//Assembles a variable declaration
void Asm_x86::build_var_dec(AstNode *node) {
	AstVarDec *vd = static_cast<AstVarDec *>(node);
	Var v = current_scope->vars[vd->get_name()];
	v.is_array = false;
	
	//Determine the stack position
	switch (v.type) {
		case DataType::Byte:
		case DataType::Char: stack_pos += 1; break;
		case DataType::Short: stack_pos += 2; break;
		case DataType::Bool:
		case DataType::Int:
		case DataType::Str:
		case DataType::Float: stack_pos += 4; break;
	}
	
	v.stack_pos = stack_pos;
	vars[vd->get_name()] = v;
	
	build_var_assign(node);
}

//Builds a variable assignment
void Asm_x86::build_var_assign(AstNode *node) {
	AstAttrNode *n = static_cast<AstAttrNode *>(node);
	Var v = vars[n->get_name()];
	
	std::string ln = "";
	std::string dest_var = "[ebp-" + std::to_string(v.stack_pos) + "]";
	bool stop = false;
	bool is_char = false;
	
	//Variable assignments
	if (node->type == AstType::VarDec || node->type == AstType::VarAssign) {
		AstVarDec *va = dynamic_cast<AstVarDec *>(node);
		if (va->get_type() == DataType::Float) {
			build_flt_assign(node);
			return;
		}
		
		if (va->get_type() == DataType::Char)
			is_char = true;
		
	//Array assignments
	} else if (node->type == AstType::ArrayAssign) {
		AstArrayAssign *assign = dynamic_cast<AstArrayAssign *>(node);
		dest_var = "[ebp-";
		int size = 4;
		
		//Math for int: 4 * the index
		switch (assign->index->type) {
			case AstType::Id: {
				AstID *i = dynamic_cast<AstID *>(assign->index);
				dest_var += std::to_string((size*v.size));
				dest_var += "+ebx]";
				
				std::string ln2 = "mov ebx, [ebp-";
				ln2 += std::to_string(v.stack_pos);
				ln2 += "]";
				
				sec_text.push_back(ln2);
				sec_text.push_back("imul ebx, " + std::to_string(size));
			} break;
			
			case AstType::Int: {
				AstInt *i = dynamic_cast<AstInt *>(assign->index);
				int val = v.stack_pos + (i->get_val() * size);
				dest_var += std::to_string(val) + "]";
			} break;
		}
	}
	
	//Build the first element
	auto first = node->children.at(0);
	
	switch (first->type) {
		//ID
		case AstType::Id: {
			AstID *id = static_cast<AstID *>(first);
			Var v2 = vars[id->get_name()];
			
			ln = "mov eax, [ebp-";
			ln += std::to_string(v2.stack_pos) + "]";
		} break;
		
		//Int
		case AstType::Int: {
			AstInt *i = static_cast<AstInt *>(first);
			auto val = std::to_string(i->get_val());
			
			ln = "mov eax, " + val;
		} break;
		
		//Characters
		case AstType::Char: {
			AstChar *ch = static_cast<AstChar *>(first);
			int val = (int)ch->get_val();
			
			ln = "mov byte " + dest_var + ", " + std::to_string(val);
			stop = true;
		} break;
		
		//Strings
		case AstType::Str: {
			auto name = build_string(first);
			
			ln = "mov dword " + dest_var + ", " + name;
			stop = true;
		} break;
		
		//Booleans
		case AstType::Bool: {
			AstBool *bl = static_cast<AstBool *>(first);
			bool val = bl->get_val();
			
			if (val)
				ln = "mov eax, 1";
			else
				ln = "mov eax, 0";
		} break;
		
		//Function calls
		case AstType::FuncCall: {
			AstFuncCall *fc = dynamic_cast<AstFuncCall *>(first);
			build_func_call(fc);
		} break;
		
		//Array access
		case AstType::ArrayAccess: {
			build_arr_access(first);
		} break;
	}
	
	sec_text.push_back(ln);
	
	//In some instances, further arguments would not be appropriate
	//In this case, we exit the function
	if (stop) {
		sec_text.push_back("");
		return;
	}
	
	//Now iterate through the reset of the children
	for (int i = 1; i<node->children.size(); i+=2) {
		auto op = node->children.at(i);
		auto next = node->children.at(i+1);
		std::string ln = "";
		
		bool is_mod = false;
		bool is_div = false;
		
		switch (op->type) {
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
			sec_text.push_back("mov " + dest_var + ", eax");
			AstFuncCall *fc = dynamic_cast<AstFuncCall *>(next);
			build_func_call(fc);
			ln += dest_var;
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
	
	//Finally, move it back
	ln = "mov ";
	
	switch (v.type) {
		case DataType::Byte:
		case DataType::Char: ln += "byte "; break;
		case DataType::Short: ln += "word "; break;
		case DataType::Bool:
		case DataType::Int:
		case DataType::Float: ln += "dword "; break;
		case DataType::Str: ln += "qword "; break;
	}
	
	std::string reg = "eax";
	
	if (is_char)
		reg = "al";
		
	ln += dest_var + ", " + reg;
	
	sec_text.push_back(ln);
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

