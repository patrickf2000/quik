#include <iostream>

#include "ltac_build.hh"
#include "ltac.hh"

LTAC_Builder::LTAC_Builder(std::string output) {
	file = new LtacFile(output);
}

//The main assembly function
void LTAC_Builder::assemble(AstNode *top) {
	for (auto node : top->children) {
		switch (node->type) {
			case AstType::Scope: assemble(node); break;
			
			case AstType::ExternFunc: build_extern(node); break;
			case AstType::FuncDec: {
				build_func_dec(node);
				assemble(node);
			} break;
			
			case AstType::Return: build_ret(node); break;
			
			case AstType::VarDec: build_var_dec(node); break;
			case AstType::VarAssign: build_var_assign(node); break;
		}
	}
}

//Increments the stack position based on variable datatype
int LTAC_Builder::size_of(DataType t) {
	switch (t) {
		case DataType::Int: return 4;
		
		//TODO: Add the rest
	}
	
	return 1;
}



