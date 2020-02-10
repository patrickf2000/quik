#include <iostream>
#include <fstream>

#include "build.hh"
#include "aasm.hh"

void build(AstNode *top, AsmNode *scope) {
	AsmNode *func = new AsmNode;
	func->type = A_Asm::Label;
	
	AsmString *str = new AsmString("myFunc");
	func->children.push_back(str);
	
	scope->children.push_back(func);
}

AsmFile *build_asm_file(AstNode *top) {
	AsmFile *file = new AsmFile("out.asm");
	build(top, file);
	
	return file;
}

void write_asm_file(AsmFile *file) {
	std::ofstream writer(file->file);

	for (AsmNode *node : file->children) {
		writer << aasm_translate(node) << std::endl;
	}
	
	writer.close();
}
