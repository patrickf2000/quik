#include "armv7.hh"

//Builds a function declaration
void Asm_Armv7::build_func_dec(AstNode *node) {
	AstFuncDec *fd = static_cast<AstFuncDec *>(node);
	sec_text.push_back(fd->get_name() + ":");

	sec_text.push_back("push {fp, lr}");
	sec_text.push_back("add fp, sp, #4");
	sec_text.push_back("");
}

//Builds a function call
void Asm_Armv7::build_func_call(AstNode *node) {
	AstFuncCall *fc = static_cast<AstFuncCall *>(node);
	
	for (auto child : fc->children) {
		switch (child->type) {
			//Integers
			case AstType::Int: {
				AstInt *i = static_cast<AstInt *>(child);
				sec_text.push_back("mov r0, #" + std::to_string(i->get_val()));
			} break;
		
			//Strings
			case AstType::Str: {
				auto name = build_string(child);
				sec_text.push_back("ldr r0, ." + name);
			} break;
			
			//Other variables
			case AstType::Id: {
				AstID *id = static_cast<AstID *>(child);
				Var v = vars[id->get_name()];
				
				std::string ln = "ldr r0, [fp, #-";
				ln += std::to_string(v.stack_pos) + "]";
				sec_text.push_back(ln);
			} break;
			
			//TODO: Add the rest
		}
	}
	
	sec_text.push_back("bl " + fc->get_name());
	sec_text.push_back("");
}

//Build an extern function call
void Asm_Armv7::build_extern_func(AstNode *node) {
	AstExternFunc *fd = static_cast<AstExternFunc *>(node);
	extern_data.push_back(".extern " + fd->get_name());
}

//Builds a return statement
void Asm_Armv7::build_ret(AstNode *node) {
	sec_text.push_back("pop {fp, pc}");
	sec_text.push_back("");
}
