#include "ltac_build.hh"
#include "ltac.hh"

LTAC_Builder::LTAC_Builder(std::string output) {
	file = new LtacFile(output);
}

void LTAC_Builder::assemble(AstNode *top) {
	
}
