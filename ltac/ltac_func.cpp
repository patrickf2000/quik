#include "ltac_build.hh"

//Builds external declarations
void LTAC_Builder::build_extern(AstNode *node) {
	AstExternFunc *fd = static_cast<AstExternFunc *>(node);

	AsmNode *a_node = new AsmNode;
	a_node->type = ltac::Extern;
	
	AsmString *a_str = new AsmString(fd->get_name());
	a_node->children.push_back(a_str);
	
	scope->add(a_node);
}

//Builds function declarations
void LTAC_Builder::build_func(AstNode *node) {
	AstFuncDec *fd = static_cast<AstFuncDec *>(node);
	
	AsmNode *a_node = new AsmNode;
	a_node->type = ltac::Label;
	
	AsmString *a_str = new AsmString(fd->get_name());
	a_node->children.push_back(a_str);
	
	scope->add(a_node);
	scope->add(ltac::Setup);
}

//Build function calls
void LTAC_Builder::build_func_call(AstNode *node) {
	AstFuncCall *fc = static_cast<AstFuncCall *>(node);
	
	//Iterate through the arguments
	for (auto arg : fc->children) {
		switch (arg->type) {
			case AstType::Str: {
				AstString *str = static_cast<AstString *>(arg);
				
				AsmNode *a_node = new AsmNode;
				a_node->type = ltac::Mov;
				
				a_node->add(ltac::Reg);
				
				std::string name = "STR" + std::to_string(str_index);
				++str_index;
				scope->str_pool[name] = str->get_val();
				
				AsmNode *str_ptr = new AsmNode;
				str_ptr->type = ltac::Ptr;
				str_ptr->val = name;
				a_node->add(str_ptr);
				
				scope->add(a_node);
			} break;
			
			//TODO: Add the rest
		}
	}
	
	//Build the actual call statement
	AsmNode *a_node = new AsmNode;
	a_node->type = ltac::Call;
	
	AsmString *a_str = new AsmString(fc->get_name());
	a_node->add(a_str);
	
	scope->add(a_node);
}

//Build return statements
void LTAC_Builder::build_ret(AstNode *node) {
	scope->add(ltac::Leave);
	scope->add(ltac::Ret);
}
