#include <iostream>
#include <fstream>

#include "build.hh"
#include "ltac.hh"
#include "ltac_func.hh"

void build(AstNode *top, AsmFile *scope) {
	for (auto node : top->children) {
		switch (node->type) {
			//Scopes
			case AstType::Scope: build(node, scope); break;
		
			//Build an extern declaration
			case AstType::ExternFunc: aasm_build_extern(node, scope); break;
			
			//Build a regular function declaration
			case AstType::FuncDec: {
				aasm_build_func(node, scope);
				build(node, scope);
			} break;
			
			//Build a function call
			case AstType::FuncCall: aasm_build_func_call(node, scope); break;
			
			//Build return statements
			case AstType::Return: aasm_build_ret(node, scope); break;
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
		if (node->type == ltac::Label)
			writer << std::endl;
		else if (node->type != ltac::Extern)
			writer << "\t";
		
		writer << ltac_translate(node) << std::endl;
	}
	
	writer.close();
}
