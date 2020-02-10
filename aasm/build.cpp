#include <iostream>
#include <fstream>

#include "build.hh"
#include "aasm.hh"
#include "aasm_func.hh"

void build(AstNode *top, AsmNode *scope) {
	for (auto node : top->children) {
		switch (node->type) {
			//Build an extern declaration
			case AstType::ExternFunc: aasm_build_extern(node, scope); break;
			
			//Build a regular function declaration
			case AstType::FuncDec: {
				aasm_build_func(node, scope);
				build(node, scope);
			} break;
		}
	}
}

AsmFile *build_asm_file(AstNode *top) {
	AsmFile *file = new AsmFile("out.asm");
	build(top, file);
	
	return file;
}

void write_asm_file(AsmFile *file) {
	std::ofstream writer(file->file);

	for (AsmNode *node : file->children) {
		if (node->type == A_Asm::Label)
			writer << std::endl;
		else if (node->type != A_Asm::Extern)
			writer << "\t";
		
		writer << aasm_translate(node) << std::endl;
	}
	
	writer.close();
}
