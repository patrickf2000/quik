#include <iostream>

#include "asm_x64.hh"

//The constructor where we set everything up
Asm_x64::Asm_x64(LtacFile *f) {
	file = f;
	writer = std::ofstream(file->name);
}

//Write out the assembly code
void Asm_x64::write() {
	writer << ".intel_syntax noprefix" << std::endl;
	writer << ".data" << std::endl;
	
	build_data(file->data);
	
	writer << std::endl;
	writer << ".text" << std::endl;
	writer << ".global main" << std::endl;
	writer << std::endl;
	
	build_code(file->code);
	
	writer << std::endl;
	writer.close();
}

//Build the data section
void Asm_x64::build_data(LtacDataSec *data) {

}

//Build the code section
void Asm_x64::build_code(LtacCodeSec *code) {
	for (auto ln : code->children) {
		switch (ln->type) {
			case ltac::Func: build_func(ln); break;
			
			case ltac::Ret: {
				writer << "\tleave" << std::endl;
				writer << "\tret" << std::endl;
				writer << std::endl;
			} break;
		}
	}
}

//Build a function declaration
void Asm_x64::build_func(LtacNode *node) {
	auto fd = static_cast<LtacFunc *>(node);
	writer << fd->name << ":" << std::endl;
	
	//Setup the stack
	writer << "\tpush rbp" << std::endl;
	writer << "\tmov rbp, rsp" << std::endl;
	writer << std::endl;
}
