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
			//Strings
			case AstType::Str: {
				auto name = build_string(child);
				sec_text.push_back("ldr r0, ." + name);
			} break;
			
			//TODO: Add the rest
		}
	}
	
	sec_text.push_back("bl " + fc->get_name());
	sec_text.push_back("");
}

//Builds a return statement
void Asm_Armv7::build_ret(AstNode *node) {
	sec_text.push_back("pop {fp, pc}");
	sec_text.push_back("");
}
