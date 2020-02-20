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
