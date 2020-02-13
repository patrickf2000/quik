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

//Builds a function call
void LTAC_Builder::build_func_call(AstNode *node) {
	AstFuncCall *fc = static_cast<AstFuncCall *>(node);
	
	//Load the function arguments
	for (auto arg : fc->children) {
		LtacNode *larg = new LtacNode(LTAC::Mov);
		LtacNode *reg = new LtacNode(LTAC::ArgReg);
		larg->args.push_back(reg);
		
		switch (arg->type) {
			case AstType::Int: {
				AstInt *i = static_cast<AstInt *>(arg);
				LtacInt *li = new LtacInt(i->get_val());
				larg->args.push_back(li);
			} break;
			
			default: larg->args.push_back(new LtacNode(LTAC::Reg));
			
			//TODO: Add the rest
		}
		
		file->children.push_back(larg);
	}
	
	//Build the call
	LtacNode *call = new LtacNode(LTAC::Call);
	call->val = fc->get_name();
	file->children.push_back(call);
}

//Builds a function return
void LTAC_Builder::build_ret(AstNode *node) {
	LtacNode *leave = new LtacNode(LTAC::Leave);
	LtacNode *ret = new LtacNode(LTAC::Ret);
	
	file->children.push_back(leave);
	file->children.push_back(ret);
}
