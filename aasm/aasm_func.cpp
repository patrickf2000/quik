#include "aasm_func.hh"

//Builds external declarations
AsmNode *aasm_build_extern(AstNode *node) {
	AstExternFunc *fd = static_cast<AstExternFunc *>(node);

	AsmNode *a_node = new AsmNode;
	a_node->type = A_Asm::Extern;
	
	AsmString *a_str = new AsmString(fd->get_name());
	a_node->children.push_back(a_str);
	
	return a_node;
}

//Builds function declarations
void aasm_build_func(AstNode *node, AsmNode *scope) {
	AstFuncDec *fd = static_cast<AstFuncDec *>(node);
	
	AsmNode *a_node = new AsmNode;
	a_node->type = A_Asm::Label;
	
	AsmString *a_str = new AsmString(fd->get_name());
	a_node->children.push_back(a_str);
	
	scope->add(a_node);
	scope->add(A_Asm::Setup);
}

//Build function calls
AsmNode *aasm_build_func_call(AstNode *node) {
	AsmNode *a_node = new AsmNode;
	return a_node;
}

//Build return statements
AsmNode *aasm_build_ret(AstNode *node) {
	AsmNode *a_node = new AsmNode;
	return a_node;
}
