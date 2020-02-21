#include "ltac_build.hh"

//Build a variable declaration
void LTAC_Builder::build_var_dec(AstNode *node) {
	auto vd = static_cast<AstVarDec *>(node);

	Var v;
	v.name = vd->get_name();
	v.type = vd->get_type();
	v.is_array = false;
	v.is_param = false;
	
	switch (v.type) {
		case DataType::Int: stack_pos += 4; break;
		case DataType::Str: stack_pos += 8; break;
		
		//TODO: Add the rest
	}
	
	v.stack_pos = stack_pos;
	vars[v.name] = v;
}

//Build a variable assignment
void LTAC_Builder::build_var_assign(AstNode *node) {
	auto va = static_cast<AstVarAssign *>(node);
	auto v = vars[va->get_name()];
	
	auto var = new LtacVar;
	var->pos = v.stack_pos;
	var->d_type = v.type;
	
	//Build the assigned value
	auto val = va->children[0];
	
	switch (val->type) {
		//Integers
		case AstType::Int: {
			auto i = static_cast<AstInt *>(val);
			auto li = new LtacInt(i->get_val());
			var->children.push_back(li);
		} break;
		
		//Strings
		case AstType::Str: {
			auto lstr = build_string(val);
			var->children.push_back(lstr);
		} break;
		
		//Other variables
		case AstType::Id: {
			auto id = static_cast<AstID *>(val);
			auto v2 = vars[id->get_name()];
			
			auto l_id = new LtacVar;
			l_id->pos = v2.stack_pos;
			var->children.push_back(l_id);
		} break;
		
		//TODO: Add the rest
	}
	
	file->code->children.push_back(var);
}
