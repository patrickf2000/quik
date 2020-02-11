#include "aasm_func.hh"

//Builds external declarations
void aasm_build_extern(AstNode *node, AsmFile *scope) {
	AstExternFunc *fd = static_cast<AstExternFunc *>(node);

	AsmNode *a_node = new AsmNode;
	a_node->type = A_Asm::Extern;
	
	AsmString *a_str = new AsmString(fd->get_name());
	a_node->children.push_back(a_str);
	
	scope->add(a_node);
}

//Builds function declarations
void aasm_build_func(AstNode *node, AsmFile *scope) {
	AstFuncDec *fd = static_cast<AstFuncDec *>(node);
	
	AsmNode *a_node = new AsmNode;
	a_node->type = A_Asm::Label;
	
	AsmString *a_str = new AsmString(fd->get_name());
	a_node->children.push_back(a_str);
	
	scope->add(a_node);
	scope->add(A_Asm::Setup);
}

//Build function calls
void aasm_build_func_call(AstNode *node, AsmFile *scope) {
	AstFuncCall *fc = static_cast<AstFuncCall *>(node);
	
	//Iterate through the arguments
	for (auto arg : fc->children) {
		switch (arg->type) {
			case AstType::Str: {
				AstString *str = static_cast<AstString *>(arg);
				
				AsmNode *a_node = new AsmNode;
				a_node->type = A_Asm::Mov;
				
				a_node->add(A_Asm::Reg);
				
				AsmNode *str_ptr = new AsmNode;
				str_ptr->type = A_Asm::Ptr;
				str_ptr->val = str->get_val();
				a_node->add(str_ptr);
				
				scope->add(a_node);
			} break;
			
			//TODO: Add the rest
		}
	}
	
	//Build the actual call statement
	AsmNode *a_node = new AsmNode;
	a_node->type = A_Asm::Call;
	
	AsmString *a_str = new AsmString(fc->get_name());
	a_node->add(a_str);
	
	scope->add(a_node);
}

//Build return statements
void aasm_build_ret(AstNode *node, AsmFile *scope) {
	scope->add(A_Asm::Leave);
	scope->add(A_Asm::Ret);
}
