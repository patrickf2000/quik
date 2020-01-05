//x86_utils
//Contains backend-specific stuff
#include "asmx86.hh"

std::map<std::string, std::string> regs32;
std::map<std::string, std::string> regs64;

void Asm_x86::init_registers() {
	regs32["ax"] = "eax";
	regs32["bx"] = "ebx";
	regs32["cx"] = "ecx";
	regs32["dx"] = "edx";
	regs32["bp"] = "ebp";
	regs32["sp"] = "esp";
	
	regs64["ax"] = "rax";
	regs64["bx"] = "rbx";
	regs64["cx"] = "rcx";
	regs64["dx"] = "rdx";
	regs64["bp"] = "rbp";
	regs64["sp"] = "rsp";
}

//Returns the proper register based on specified architecture
std::string Asm_x86::get_reg(std::string r) {
	if (x64)
		return regs64[r];
	return regs32[r];
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
			Var v2 = vars[id->get_name()];
			
			ln = "[" + get_reg("bp") + "-";
			ln += std::to_string(v2.stack_pos) + "]";
		} break;
		
		case AstType::Char: {
			AstChar *ch = dynamic_cast<AstChar *>(node);
			ln = "\'";
			ln += ch->get_val();
			ln += "\'";
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

//Builds a string and inserts it in the data section
std::string Asm_x86::build_string(AstNode *node) {
	AstString *s = static_cast<AstString *>(node);
	
	//Check for escape characters
	std::string old_val = s->get_val();
	std::string val = "\"";
	bool done = false;
	
	for (int i = 0; i<old_val.length(); i++) {
		if (old_val[i] == '\\' && old_val[i+1] == 'n') {
			val += "\",0xA";
			
			if (i+2 >= old_val.length()) {
				done = true;
				break;
			}
				
			val += ",\"";
			i += 1;
		} else {
			val += old_val[i];
		}
	}
	
	if (!done)
		val += '\"';
		
	//Build the final product
	std::string name = "STR_" + std::to_string(str_index);
	std::string str = name + " db " + val + ",0";
	++str_index;
	sec_data.push_back(str);
	
	return name;
}

//Builds a floating-point number and inserts it in the data section
std::string Asm_x86::build_float(AstNode *node) {
	AstFloat *flt = static_cast<AstFloat *>(node);
	double val = flt->get_val();
	
	std::string name = "flt_" + std::to_string(flt_index);
	++flt_index;
	
	std::string d_ln = name + " dq " + std::to_string(val);
	sec_data.push_back(d_ln);
	
	return name;
}

