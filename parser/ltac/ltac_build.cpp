#include "ltac_build.hh"

//Creates an LTAC file
LtacFile *LTAC_Builder::build_file(AstNode *top) {
	LtacFile *file = new LtacFile("out");
	
	auto data = new LtacDataSec;
	auto code = new LtacCodeSec;
	
	file->data = data;
	file->code = code;
	
	assemble(top, file);
	
	return file;
}

//The main assembly function for ltac
void LTAC_Builder::assemble(AstNode *top, LtacFile *file) {
	for (auto node : top->children) {
	
	}
}
