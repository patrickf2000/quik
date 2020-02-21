#include "ltac_build.hh"

//Creates an LTAC file
LtacFile *LTAC_Builder::build_file(AstNode *top) {
	file = new LtacFile("out");
	
	auto data = new LtacDataSec;
	auto code = new LtacCodeSec;
	
	file->data = data;
	file->code = code;
	
	assemble(top);
	
	check_overload();
	
	return file;
}

//The main assembly function for ltac
void LTAC_Builder::assemble(AstNode *top) {
	for (auto node : top->children) {
		switch (node->type) {
			case AstType::Scope: assemble(node); break;
			
			case AstType::ExternFunc: build_func(node, true); break;
			
			case AstType::FuncDec: {
				build_func(node);
				assemble(node);
			} break;
			
			case AstType::Return: {
				auto rnode = new LtacNode(ltac::Ret);
				file->code->children.push_back(rnode);
				stack_pos = 0;
			} break;
			
			case AstType::FuncCall: build_func_call(node); break;
			
			case AstType::VarDec: build_var_dec(node);
			case AstType::VarAssign: build_var_assign(node); break;
		}
	}
}

//Builds a string variable (utility function)
LtacNode *LTAC_Builder::build_string(AstNode *node) {
	auto str = static_cast<AstString *>(node);
	auto lstr = new LtacString;
	std::string val = str->get_val();

	if (dec_strings.find(val) != dec_strings.end()) {
		lstr->name = dec_strings[val];
		lstr->val = val;
		return lstr;
	}

	std::string name = "STR_" + std::to_string(str_count);
	++str_count;

	lstr->name = name;
	lstr->val = str->get_val();
	
	dec_strings[lstr->val] = lstr->name;

	file->data->children.push_back(lstr);
	return lstr;
}

//Increments the stack based on a datatype
void LTAC_Builder::inc_stack(DataType type) {
	switch (type) {
		case DataType::Bool:
		case DataType::Int: stack_pos += 4; break;
		case DataType::Str: stack_pos += 8; break;
		
		//TODO: Add the rest
	}
}


