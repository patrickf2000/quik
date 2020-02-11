#include <iostream>
#include <fstream>

#include "build.hh"
#include "ltac.hh"
#include "ltac_build.hh"

AsmFile *build_asm_file(AstNode *top) {
	AsmFile *file = new AsmFile("out.asm");
	LTAC_Builder *builder = new LTAC_Builder(file);
	builder->build(top);
	
	return file;
}

void write_asm_file(AsmFile *file) {
	std::ofstream writer(file->file);
	
	//Write the data
	for (auto const& str : file->str_pool) {
		writer << str.first << ": \"" << str.second << "\"" << std::endl;
	}
	writer << std::endl;

	//Write the code
	for (AsmNode *node : file->children) {
		if (node->type == ltac::Label)
			writer << std::endl;
		else if (node->type != ltac::Extern)
			writer << "\t";
		
		writer << ltac_translate(node) << std::endl;
	}
	
	writer.close();
}
