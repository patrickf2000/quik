#include <iostream>
#include <fstream>

#include "build.hh"
#include "aasm.hh"
#include "aasm_func.hh"

void build(AstNode *top, AsmNode *scope) {
	for (auto node : top->children) {
		switch (node->type) {
			//Build an extern declaration
			case AstType::ExternFunc: {
				auto current = aasm_build_extern(node); 
				scope->children.push_back(current);
			} break;
			
			//Build a regular function declaration
			case AstType::FuncDec: {
				auto current = aasm_build_func(node);
				scope->children.push_back(current);
				
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
		
		writer << aasm_translate(node) << std::endl;
	}
	
	writer.close();
}
