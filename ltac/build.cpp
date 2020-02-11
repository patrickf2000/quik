#include <iostream>
#include <fstream>

#include "build.hh"
#include "ltac.hh"
#include "ltac_build.hh"

void build(AstNode *top, LTAC_Builder *b) {
	for (auto node : top->children) {
		switch (node->type) {
			//Scopes
			case AstType::Scope: build(node, b); break;
		
			//Build an extern declaration
			case AstType::ExternFunc: b->build_extern(node); break;
			
			//Build a regular function declaration
			case AstType::FuncDec: {
				b->build_func(node);
				build(node, b);
			} break;
			
			//Build a function call
			case AstType::FuncCall: b->build_func_call(node); break;
			
			//Build return statements
			case AstType::Return: b->build_ret(node); break;
		}
	}
}

AsmFile *build_asm_file(AstNode *top) {
	AsmFile *file = new AsmFile("out.asm");
	LTAC_Builder *builder = new LTAC_Builder(file);
	build(top, builder);
	
	return file;
}

void write_asm_file(AsmFile *file) {
	std::ofstream writer(file->file);

	for (AsmNode *node : file->children) {
		if (node->type == ltac::Label)
			writer << std::endl;
		else if (node->type != ltac::Extern)
			writer << "\t";
		
		writer << ltac_translate(node) << std::endl;
	}
	
	writer.close();
}
