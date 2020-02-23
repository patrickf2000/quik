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
				auto func = build_func(node);
				assemble(node);
				
				int stack_size = 0;
				if (stack_pos > 0) {
					while (stack_size < (stack_pos + 1))
						if (stack_size < 16)
							stack_size += 8;
						else
							stack_size += 16;
				}
				
				func->stack_size = stack_size;
				stack_pos = 0;
			} break;
			
			case AstType::Return: {
				auto rnode = new LtacNode(ltac::Ret);
				file->code->children.push_back(rnode);
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

//Builds a float variable (utility function)
LtacNode *LTAC_Builder::build_float(AstNode *node) {
	auto flt = static_cast<AstFloat *>(node);
	auto l_flt = new LtacFloat;
	float val = flt->get_val();
	
	std::string name = "FLT_" + std::to_string(flt_count);
	++flt_count;
	
	l_flt->name = name;
	l_flt->val = val;
	
	char buf[32];
	sprintf(buf, "%d", *(unsigned int*)&val);
	l_flt->i_val = std::stoi(std::string(buf));
	
	dec_flt[val] = l_flt->name;
	
	file->data->children.push_back(l_flt);
	return l_flt;
}

//Increments the stack based on a datatype
void LTAC_Builder::inc_stack(DataType type) {
	switch (type) {
		case DataType::Byte:
		case DataType::Char: stack_pos += 1; break;
		case DataType::Bool:
		case DataType::Int: 
		case DataType::Float: stack_pos += 4; break;
		case DataType::Str: stack_pos += ptr_size; break;
		
		//TODO: Add the rest
	}
}


