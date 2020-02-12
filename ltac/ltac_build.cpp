#include "ltac_build.hh"
#include "ltac.hh"

LTAC_Builder::LTAC_Builder(std::string output) {
	file = new LtacFile(output);
}

//The main assembly function
void LTAC_Builder::assemble(AstNode *top) {
	for (auto node : top->children) {
		switch (node->type) {
			case AstType::Scope: assemble(node); break;
			
			case AstType::ExternFunc: build_extern(node); break;
			case AstType::FuncDec: build_func_dec(node); break;
		}
	}
}

//Builds an extern function declaration
void LTAC_Builder::build_extern(AstNode *node) {
	AstAttrNode *fd = static_cast<AstAttrNode *>(node);
	
	LtacNode *lbl = new LtacNode(LTAC::Extern);
	lbl->val = fd->get_name();
	
	file->children.push_back(lbl);
}

//Builds a function declaration
void LTAC_Builder::build_func_dec(AstNode *node) {
	AstFuncDec *fd = static_cast<AstFuncDec *>(node);
	
	LtacNode *lbl = new LtacNode(LTAC::Label);
	lbl->val = fd->get_name();
	
	LtacNode *setup = new LtacNode(LTAC::Setup);
	
	file->children.push_back(lbl);
	file->children.push_back(setup);
}

