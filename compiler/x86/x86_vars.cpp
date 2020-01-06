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
		case DataType::Str: stack_pos += 4; break;
		case DataType::Float: stack_pos += 8; break;
	}
	
	v.stack_pos = stack_pos;
	vars[vd->get_name()] = v;
	
	//Determine the nature of the first variable
	if (vd->children.size() > 1 || vd->type == AstType::ArrayAssign) {
		build_var_assign(node);
		return;
	}
	
	auto child = vd->children.at(0);
	
	switch (child->type) {
		case AstType::FuncCall:
		case AstType::ArrayAccess:
		case AstType::Id: {
			build_var_assign(node);
			return;
		} break;
		case AstType::Float: {
			build_flt_assign(node);
			return;
		} break;
	}
	
	//Build the first variable
	std::string dest_var = "[" + get_reg("bp") + "-";
	dest_var += std::to_string(v.stack_pos) + "]";
	std::string ln = "";
	
	if (child->type == AstType::Inc) {
		ln = "add dword " + dest_var + ", 1";
	} else {
		ln = "mov " + asm_type(v);
		ln += " " + dest_var + ", " + type2asm(child);
	}
	
	sec_text.push_back(ln);
	sec_text.push_back("");
}

//Builds a variable assignment
void Asm_x86::build_var_assign(AstNode *node) {
	AstAttrNode *n = static_cast<AstAttrNode *>(node);
	Var v = vars[n->get_name()];
	
	//Construct the destination variable
	std::string dest_var = "[" + get_reg("bp") + "-";
	dest_var += std::to_string(v.stack_pos) + "]";
	
	std::string ln = "";
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
		dest_var = "[" + get_reg("bp") + "-";
		int size = 4;
		
		//Math for int: 4 * the index
		switch (assign->index->type) {
			case AstType::Id: {
				AstID *i = dynamic_cast<AstID *>(assign->index);
				Var v2 = vars[i->get_name()];
				
				dest_var += std::to_string((size*v.size));
				dest_var += "+" + get_reg("bx") + "]";
				
				std::string ln2 = "mov ebx, [" + get_reg("bp") + "-";
				ln2 += std::to_string(v2.stack_pos);
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
			
			ln = "mov eax, [" + get_reg("bp") + "-";
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
		
		//Increment
		case AstType::Inc: {
			ln = "add dword " + dest_var + ", 1";
			stop = true;
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
		auto op = node->children[i];
		auto next = node->children[i+1];
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
		case DataType::Str: ln += "dword "; break;
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
	AstAttrNode *va = static_cast<AstAttrNode *>(node);
	Var v = vars[va->get_name()];
	
	auto first = va->children.at(0);
	
	switch (first->type) {
		//Assign a float value
		case AstType::Float: {
			auto name = build_float(first);
			sec_text.push_back("movq xmm0, [" + name + "]");
		} break;
		
		//Assign another variable
		case AstType::Id: {
			AstID *id = static_cast<AstID *>(first);
			Var v2 = vars[id->get_name()];
			
			std::string src = "movq xmm0, [" + get_reg("bp");
			src += "-" + std::to_string(v2.stack_pos) + "]";
			sec_text.push_back(src);
		} break;
	}
	
	//If there are further children, we have math
	for (int i = 1; i<va->children.size(); i+=2) {
		auto op = va->children[i];
		auto next = va->children[i+1];
		std::string ln2 = "";
		
		switch (op->type) {
			case AstType::Add: ln2 = "addsd xmm0, "; break;
			case AstType::Sub: ln2 = "subsd xmm0, "; break;
			case AstType::Mul: ln2 = "mulsd xmm0, "; break;
			case AstType::Div: ln2 = "divsd xmm0, "; break;
		}
		
		switch (next->type) {
			case AstType::Float: {
				auto name = build_float(next);
				ln2 += "[" + name + "]";
			} break;
			
			case AstType::Id: {
				AstID *id = static_cast<AstID *>(next);
				Var v2 = vars[id->get_name()];
				
				ln2 += "[" + get_reg("bp") + "-";
				ln2 += std::to_string(v2.stack_pos) + "]";
			} break;
		}
		
		sec_text.push_back(ln2);
	}
	
	//Push the final result to the stack
	std::string ln = "movsd [" + get_reg("bp") + "-";
	ln += std::to_string(v.stack_pos) + "], xmm0";
			
	sec_text.push_back(ln);
	sec_text.push_back("");
}

