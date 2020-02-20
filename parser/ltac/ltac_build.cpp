#include "ltac_build.hh"

//The main assembly function for ltac
LtacFile *LTAC_Builder::build_file(AstNode *top) {
	LtacFile *file = new LtacFile("out");
	return file;
}
