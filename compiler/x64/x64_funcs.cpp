#include "asm_x64.hh"

//Build a function declaration
void Asm_x64::build_func(LtacNode *node) {
	auto fd = static_cast<LtacFunc *>(node);
	writer << fd->name << ":" << std::endl;
	
	//Setup the stack
	writer << "\tpush rbp" << std::endl;
	writer << "\tmov rbp, rsp" << std::endl;
	writer << std::endl;
}
