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
	
	//See if we have an integer math chain
	if (v.type == DataType::Int && va->children.size() > 1)
		build_int_chain(va);
	
	//Store the variable
	std::string ln = "str r2, [fp, #-";
	ln += std::to_string(v.stack_pos) + "]";
	sec_text.push_back(ln);
	sec_text.push_back("");
}

//Build an integer chain
void Asm_Armv7::build_int_chain(AstVarAssign *va) {
	auto children = va->children;
	std::string val = "";
	
	for (int i = 1; i<children.size(); i+=2) {
		auto op = children[i];
		auto next = children[i+1];
		val = "";
		
		//Determine what the next value is
		switch (next->type) {
			//Integers
			case AstType::Int: {
				AstInt *i = static_cast<AstInt *>(next);
				val = "#" + std::to_string(i->get_val());
			} break;
			
			//Other variables
			case AstType::Id: {
				AstID *id = static_cast<AstID *>(next);
				Var v = vars[id->get_name()];
				std::string pos = std::to_string(v.stack_pos);
				
				sec_text.push_back("ldr r3, [fp, #-" + pos + "]");
				val = "r3";
			} break;
		}
		
		//Determine the operator type
		switch (op->type) {
			//Addition
			case AstType::Add: {
				sec_text.push_back("add r2, r2, " + val);
			} break;
			
			//Subtraction
			case AstType::Sub: {
				sec_text.push_back("sub r2, r2, " + val);
			} break;
			
			//Multiplication
			case AstType::Mul: {
				if (val[0] == '#')
					sec_text.push_back("mov r3, " + val);
				sec_text.push_back("mul r4, r2, r3");
				sec_text.push_back("mov r2, r4");
			} break;
			
			//Division
			case AstType::Div: {
				//TODO: Add me
			} break;
			
			//Mod
			case AstType::Mod: {
				//TODO: Add me
			} break;
		}
	}
}

