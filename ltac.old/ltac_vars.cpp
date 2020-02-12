#include "ltac_build.hh"

//Build variable declarations
void LTAC_Builder::build_var_dec(AstNode *node) {
	AstVarDec *vd = static_cast<AstVarDec *>(node);
	Var v = var_scope->vars[vd->get_name()];
	v.is_array = false;
	
	//Determine the stack position
	switch (v.type) {
		case DataType::Byte:
		case DataType::Char: stack_pos += 1; break;
		case DataType::Short: stack_pos += 2; break;
		case DataType::Bool:
		case DataType::Int:
		case DataType::Float:
		case DataType::Str: stack_pos += 4; break;
		case DataType::Int64: 
		case DataType::Float64: {
			stack_pos += 8;
			v.size = 2;
		} break;
		case DataType::Int128:
		case DataType::Float128: {
			stack_pos += 16;
			v.size = 4;
		} break;
		case DataType::Int256:
		case DataType::Float256: {
			stack_pos += 32;
			v.size = 8;
		} break;
	}
	
	v.stack_pos = stack_pos;
	vars[vd->get_name()] = v;
	
	build_var_assign(node);
}

//Build variable assignments
void LTAC_Builder::build_var_assign(AstNode *node) {
	AstVarDec *vd = static_cast<AstVarDec *>(node);
	Var v = vars[vd->get_name()];
	
	/*if (vd->get_type() == DataType::Float) {
		build_flt_assign(node);
		return;
	} else if (vd->get_type() == DataType::Int64
		|| vd->get_type() == DataType::Float64 
		|| vd->get_type() == DataType::Int128
		|| vd->get_type() == DataType::Float128
		|| vd->get_type() == DataType::Int256
		|| vd->get_type() == DataType::Float256) {
		build_floatex_assign(node);
		return;	
	}*/
	
	//Build the first variable
	auto child = vd->children.at(0);
	
	//Array assignments
	if (node->type == AstType::ArrayAssign) {
		
	//Structure assignments
	} else if (node->type == AstType::StructMod) {

	}
	
	//Increments
	if (child->type == AstType::Inc) {
		
		
	//Other variables
	} else if (child->type == AstType::Id) {
		AstID *id = static_cast<AstID *>(child);
		Var v2 = vars[id->get_name()];
	
		//Build the first line (move the variable we want to a register)
		AsmNode *a_node1 = new AsmNode;
		a_node1->type = ltac::Mov;
		
		AsmReg *reg = new AsmReg(0);
		a_node1->add(reg);
		
		AsmMem *mem1 = new AsmMem(v2.stack_pos, 0);
		mem1->val = v2.name;
		a_node1->add(mem1);
		
		//Build the second line (move the register to the current var)
		AsmNode *a_node2 = new AsmNode;
		a_node2->type = ltac::Mov;
		
		AsmMem *mem2 = new AsmMem(v.stack_pos, 0);
		mem2->val = v.name;
		a_node2->add(mem2);
		
		a_node2->add(reg);
		
		//Add both lines
		scope->add(a_node1);
		scope->add(a_node2);
		scope->add_nl();
		
	//Function calls
	} else if (child->type == AstType::FuncCall) {
		
		
	//Array access
	} else if (child->type == AstType::ArrayAccess) {
		
		
	//Struct access
	} else if (child->type == AstType::StructAcc) {
		
		
	//Integer math
	} else if (child->type == AstType::Math) {
		if (v.type == DataType::Int) {
		}
		
	//Raw types
	} else {
		AsmNode *a_node = new AsmNode;
		a_node->type = ltac::Mov;
		
		switch (child->type) {
			case AstType::Int: {
				AstInt *i = static_cast<AstInt *>(child);
			
				AsmMem *a_mem = new AsmMem(v.stack_pos, 0);
				a_mem->val = v.name;
				a_node->add(a_mem);
				
				AsmNode *a_num = new AsmNode;
				a_num->type = ltac::No;
				a_num->val = std::to_string(i->get_val());
				a_node->add(a_num);
			} break;
		}
		
		scope->add(a_node);
		scope->add_nl();
	}
}
