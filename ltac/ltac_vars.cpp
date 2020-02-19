#include "ltac_build.hh"

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
		//Boolean values
		case AstType::Bool: {
			AstBool *bl = static_cast<AstBool *>(first);
			int val = 0;
			if (bl->get_val()) val = 1;
			
			LtacInt *li = new LtacInt(val);
			node->args.push_back(li);
		} break;
		
		//Unsigned byte values
		case AstType::Hex: {
			auto *hex = static_cast<AstHex *>(first);
		} break;
	
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
