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
	
	//TODO: Add arguments
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
				auto str = static_cast<AstString *>(arg);
				auto lstr = new LtacString;
				
				std::string name = "STR_" + std::to_string(str_count);
				++str_count;
				
				lstr->name = name;
				lstr->val = str->get_val();
				
				l_fc->children.push_back(lstr);
				file->data->children.push_back(lstr);
			} break;
			
			//TODO: Add the rest
		}
	}
}

