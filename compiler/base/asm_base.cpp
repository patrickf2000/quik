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
	
	buildData(file);
	
	writer << std::endl;
	writer << ".text" << std::endl;
	writer << std::endl;
	
	buildCode(file);
	
	writer << std::endl;
	writer.close();
}
