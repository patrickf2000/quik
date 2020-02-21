#include "ltac_build.hh"

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
			//Push an integer
			case AstType::Int: {
				auto i = static_cast<AstInt *>(arg);
				auto li = new LtacInt(i->get_val());
				l_fc->children.push_back(li);
			} break;
		
			//Push a string
			case AstType::Str: {
				auto lstr = build_string(arg);
				l_fc->children.push_back(lstr);
			} break;
			
			//Push another variable
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

