#include "asm_arm7.hh"

//Builds a function declaration
void Asm_Arm7::build_func(LtacNode *node) {
	auto fd = static_cast<LtacFunc *>(node);
	
	if (fd->name == "main") {
		writer << ".global main" << std::endl;
	}
	
	writer << std::endl;
	writer << fd->name << ":" << std::endl;
	
	writer << "\tpush {fp, lr}" << std::endl;
	writer << "\tadd fp, sp, #4" << std::endl;
	writer << "\tsub sp, sp, #16" << std::endl;
	writer << std::endl;
}

//Builds function calls
void Asm_Arm7::build_func_call(LtacNode *node) {
	auto fc = static_cast<LtacFuncCall *>(node);
	int call_index = 0;
	
	//Build the arguments
	for (auto arg : fc->children) {
		switch (arg->type) {
			//Strings
			case ltac::String: {
				auto str = static_cast<LtacString *>(arg);
				writer << "\tldr r" << std::to_string(call_index);
				writer << ", " << str->name << std::endl;
				++call_index;
			} break;
			
			//Variables
			case ltac::Var: {
				auto var = static_cast<LtacVar *>(arg);
				writer << "\tldr r" << std::to_string(call_index);
				writer << ", [fp, #-" << std::to_string(var->pos);
				writer << "]" << std::endl;
			} break;
			
			//TODO: Add the rest
		}
	}
	
	//Finally, build the call
	writer << "\tbl " << fc->name << std::endl;
	writer << std::endl;
}

//Builds a function return
void Asm_Arm7::build_ret(LtacNode *node) {
	writer << "\tmov r0, #0" << std::endl;
	writer << "\tsub sp, fp, #4" << std::endl;
	writer << "\tpop {fp, pc}" << std::endl;
	writer << std::endl;
}
