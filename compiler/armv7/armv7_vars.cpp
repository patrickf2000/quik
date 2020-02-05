#include "armv7.hh"

//Build a variable declaration
void Asm_Armv7::build_var_dec(AstNode *node) {
	AstVarDec *vd = static_cast<AstVarDec *>(node);
	
	Var v;
	v.name = vd->get_name();
	v.type = vd->get_type();
	v.stack_pos = stack_pos;
	
	switch (vd->get_type()) {
		case DataType::Int: stack_pos += 4; break;
		
		//TODO: Add rest
	}
	
	vars[v.name] = v;
	build_var_assign(node);
}

//Build a variable assignment
void Asm_Armv7::build_var_assign(AstNode *node) {
	AstVarAssign *va = static_cast<AstVarAssign *>(node);
	Var v = vars[va->get_name()];
	
	//Build the initial value
	auto first = va->children.at(0);
	
	switch (first->type) {
		//Integers
		case AstType::Int: {
			AstInt *i = static_cast<AstInt *>(first);
			auto val = std::to_string(i->get_val());
			sec_text.push_back("mov r2, #" + val);
		} break;
		
		//Other variables
		case AstType::Id: {
			AstID *id = static_cast<AstID *>(first);
			Var v2 = vars[id->get_name()];
			
			std::string ln = "ldr r2, [fp, #-";
			ln += std::to_string(v2.stack_pos) + "]";
			sec_text.push_back(ln);
		} break;
		
		//TODO: Add rest
	}
	
	//Store the variable
	std::string ln = "str r2, [fp, #-";
	ln += std::to_string(v.stack_pos) + "]";
	sec_text.push_back(ln);
	sec_text.push_back("");
}
