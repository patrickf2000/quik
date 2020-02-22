#include "asm_arm7.hh"

//Build the data section
void Asm_Arm7::build_data(LtacDataSec *data) {

}

//Build the code section
void Asm_Arm7::build_code(LtacCodeSec *code) {
	for (auto ln : code->children) {
		switch (ln->type) {
			case ltac::Func: build_func(ln); break;
			
			case ltac::Ret: {
				writer << "\tmov r0, #0" << std::endl;
				writer << "\tpop {fp, pc}" << std::endl;
				writer << std::endl;
			} break;
		}
	}
}

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
	writer << std::endl;
}
