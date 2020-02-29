#include "ltac_build.hh"

//Build array declarations
void LTAC_Builder::build_array_dec(AstNode *node) {
	auto array_dec = static_cast<AstArrayDec *>(node);
	auto l_array = new LtacArray;
	
	int size = array_dec->get_size();
	stack_pos += 4 * size;
	
	l_array->stack_pos = stack_pos;
	l_array->size = size;
	l_array->type_size = 4;
	l_array->d_type = array_dec->get_type();
	
	for (auto child : array_dec->children) {
		auto node = convert_ast_var(child);
		l_array->children.push_back(node);
	}
	
	file->code->children.push_back(l_array);
	
	//Save the array to the variables array
	Var v;
	v.name = array_dec->get_name();
	v.type = array_dec->get_type();
	v.is_array = true;
	v.is_param = false;
	v.size = size;
	v.stack_pos = stack_pos;
	vars[v.name] = v;
}

//Builds array access
LtacArrayAcc *LTAC_Builder::build_array_acc(AstNode *node) {
	auto ast_acc = static_cast<AstArrayAcc *>(node);
	auto acc = new LtacArrayAcc;
	
	Var v = vars[ast_acc->get_name()];
	acc->stack_pos = v.stack_pos;
	acc->type_size = 4;
	acc->d_type = v.type;
	
	auto index = ast_acc->children[0];
	
	switch (index->type) {
		//Integer being used for index
		case AstType::Int: {
			auto i = static_cast<AstInt *>(index);
			auto li = new LtacInt(i->get_val());
			
			acc->children.push_back(li);
		} break;
		
		//Variable being used for index
		case AstType::Id: {
		
		} break;
	}
	
	return acc;
}
