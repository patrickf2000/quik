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
			
			case AstType::VarDec: build_var_dec(node); break;
			case AstType::VarAssign: build_var_assign(node); break;
		}
	}
}

//Builds an extern function declaration
void LTAC_Builder::build_extern(AstNode *node) {
	AstAttrNode *fd = static_cast<AstAttrNode *>(node);
	
	LtacNode *lbl = new LtacNode(LTAC::Extern);
	lbl->val = fd->get_name();
	
	file->children.push_back(lbl);
}

//Builds a function declaration
void LTAC_Builder::build_func_dec(AstNode *node) {
	AstFuncDec *fd = static_cast<AstFuncDec *>(node);
	
	LtacNode *lbl = new LtacNode(LTAC::Label);
	lbl->val = fd->get_name();
	
	LtacNode *setup = new LtacNode(LTAC::Setup);
	
	file->children.push_back(lbl);
	file->children.push_back(setup);
}

//Build a variable declaration
void LTAC_Builder::build_var_dec(AstNode *node) {
	AstVarDec *vd = static_cast<AstVarDec *>(node);
	Var v;
	v.name = vd->get_name();
	v.type = vd->get_type();
	v.stack_pos = stack_pos;
	v.is_param = false;
	v.is_array = false;
	vars[vd->get_name()] = v;
	
	stack_pos += size_of(v.type);
	
	build_var_assign(vd, v);
}

//Builds a variable assignment
void LTAC_Builder::build_var_assign(AstNode *node) {
	AstVarDec *va = static_cast<AstVarDec *>(node);
	Var v = vars[va->get_name()];
	build_var_assign(va, v);
}

void LTAC_Builder::build_var_assign(AstVarDec *va, Var v) {
	LtacNode *node = new LtacNode(LTAC::Mov);
	
	LtacMem *mem = new LtacMem;
	mem->index = v.stack_pos;
	mem->scale = 0;
	node->args.push_back(mem);

	auto first = va->children.at(0);
	
	switch (first->type) {
		//Integer values
		case AstType::Int: {
			AstInt *i = static_cast<AstInt *>(first);
			LtacInt *li = new LtacInt(i->get_val());
			node->args.push_back(li);
		} break;
		
		//TODO: Add the rest
	}
	
	file->children.push_back(node);
	file->children.push_back(new LtacNode(LTAC::None));
}

//Increments the stack position based on variable datatype
int LTAC_Builder::size_of(DataType t) {
	switch (t) {
		case DataType::Int: return 4;
		
		//TODO: Add the rest
	}
	
	return 1;
}



