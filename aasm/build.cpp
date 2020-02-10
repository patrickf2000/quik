#include "build.hh"
#include "aasm.hh"

AsmFile *build_asm_file(AstNode *top) {
	AsmFile *file = new AsmFile("out.asm");
	return file;
}

void write_asm_file(AsmFile *file) {

}
