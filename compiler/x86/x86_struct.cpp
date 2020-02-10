#include "asmx86.hh"

void Asm_x86::build_struct_dec(AstNode *node) {
	AstStructDec *dec = static_cast<AstStructDec *>(node);
	av_structs[dec->get_name()] = dec;
}

void Asm_x86::build_struct_var(AstNode *node) {
	AstStruct *var_str = static_cast<AstStruct *>(node);
	AstStructDec *dec = av_structs[var_str->str_name];
	
	StructV var;
	var.name = var_str->var_name;
	
	for (auto p : dec->children) {
		AstVarDec *part = static_cast<AstVarDec *>(p);
		auto child = part->children.at(0);
		
		Var v;
		v.name = part->get_name();
		
		switch (child->type) {
			case AstType::Int: {
				var.size += 4;
				stack_pos += 4;
				v.stack_pos = stack_pos;
				v.type = DataType::Int;
				
				AstInt *i = static_cast<AstInt *>(child);
				
				std::string ln = "mov DWORD PTR [" + get_reg("bp");
				ln += "-" + std::to_string(stack_pos) + "], ";
				ln += std::to_string(i->get_val());
				sec_text.push_back(ln);
			} break;
			
			//TODO: Add remaining types
		}
		
		var.vars[v.name] = v;
	}
	
	structs[var.name] = var;
	
	sec_text.push_back("");
}

//Build struct access
void Asm_x86::build_struct_acc(AstNode *node) {
	Var sub_var = get_struct_child(node);
	
	std::string ln = "mov eax, [" + get_reg("bp");
	ln += "-" + std::to_string(sub_var.stack_pos) + "]";
	
	sec_text.push_back(ln);
	sec_text.push_back("");
}

//Build struct member modification
//Returns the memory location of the structure variable
std::string Asm_x86::build_struct_mod(AstNode *node) {
	Var sub_var = get_struct_child(node);
	
	std::string ln = "[" + get_reg("bp") + "-";
	ln += std::to_string(sub_var.stack_pos) + "]";
	
	return ln;
}

Var Asm_x86::get_struct_child(AstNode *node) {
	AstStructMod *mod = static_cast<AstStructMod *>(node);
	StructV var = structs[mod->str_name];
	Var sub_var = var.vars[mod->var_name];
	return sub_var;
}


