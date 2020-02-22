#include "asm_base.hh"

AsmGen::AsmGen(LtacFile *f) {
	file = f;
	writer = std::ofstream(file->name);
}

//Write out the base assembly code
void AsmGen::write(bool x86) {
	if (x86)
		writer << ".intel_syntax noprefix" << std::endl;
	writer << ".data" << std::endl;
	
	build_data(file->data);
	
	writer << std::endl;
	writer << ".text" << std::endl;
	writer << std::endl;
	
	build_code(file->code);
	
	writer << std::endl;
	writer.close();
}
