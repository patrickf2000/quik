#include "ltac_build.hh"

//Creates an LTAC file
LtacFile *LTAC_Builder::build_file(AstNode *top) {
	file = new LtacFile("out");
	
	auto data = new LtacDataSec;
	auto code = new LtacCodeSec;
	
	file->data = data;
	file->code = code;
	
	assemble(top);
	
	return file;
}

//The main assembly function for ltac
void LTAC_Builder::assemble(AstNode *top) {
	for (auto node : top->children) {
		switch (node->type) {
			case AstType::Scope: assemble(node); break;
			
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

//Builds a function
void LTAC_Builder::build_func(AstNode *node) {
	auto fd = static_cast<AstFuncDec *>(node);
	auto l_fd = new LtacFunc(fd->get_name());
	file->code->children.push_back(l_fd);
	
	l_fd->is_global = fd->is_global;
	
	//Add the arguments
	for (auto v : fd->args) {
		inc_stack(v.type);
		v.stack_pos = stack_pos;
		vars[v.name] = v;
		
		auto var = new LtacVar;
		var->pos = stack_pos;
		var->d_type = v.type;
		
		l_fd->children.push_back(var);
	}
}

//Builds a function call
void LTAC_Builder::build_func_call(AstNode *node) {
	auto fc = static_cast<AstFuncCall *>(node);
	auto l_fc = new LtacFuncCall(fc->get_name());
	file->code->children.push_back(l_fc);
	
	//Build the arguments
	for (auto arg : fc->children) {
		switch (arg->type) {
			case AstType::Str: {
				auto lstr = build_string(arg);
				l_fc->children.push_back(lstr);
			} break;
			
			case AstType::Id: {
				auto id = static_cast<AstID *>(arg);
				auto v = vars[id->get_name()];
				
				auto var = new LtacVar;
				var->pos = v.stack_pos;
				var->d_type = v.type;
				l_fc->children.push_back(var);
			} break;
			
			//TODO: Add the rest
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
		case DataType::Int: stack_pos += 4; break;
		case DataType::Str: stack_pos += 8; break;
		
		//TODO: Add the rest
	}
}


