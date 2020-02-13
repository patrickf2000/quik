#include "ltac_build.hh"

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

//Builds a function return
void LTAC_Builder::build_ret(AstNode *node) {
	LtacNode *leave = new LtacNode(LTAC::Leave);
	LtacNode *ret = new LtacNode(LTAC::Ret);
	
	file->children.push_back(leave);
	file->children.push_back(ret);
}
