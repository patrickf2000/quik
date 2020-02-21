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
	
	build_code(file->code);
	
	writer << std::endl;
	writer.close();
}

//Build the data section
void Asm_x64::build_data(LtacDataSec *data) {

}

//Build the code section
void Asm_x64::build_code(LtacCodeSec *code) {

}
