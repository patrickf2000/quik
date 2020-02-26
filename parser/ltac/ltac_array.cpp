#include "ltac_build.hh"

//Build array declarations
void LTAC_Builder::build_array_dec(AstNode *node) {
	auto array_dec = static_cast<AstArrayDec *>(node);
	auto l_array = new LtacArray;
	
	int size = array_dec->get_size();
	stack_pos += 4 * size;
	
	l_array->stack_pos = stack_pos;
	l_array->size = size;
	l_array->d_type = array_dec->get_type();
	
	for (auto child : array_dec->children) {
		auto node = convert_ast_var(child);
		l_array->children.push_back(node);
	}
	
	file->code->children.push_back(l_array);
}
