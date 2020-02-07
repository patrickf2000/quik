#include "armv7.hh"

//Build a conditional comparison
void Asm_Armv7::build_conditional(AstNode *node) {
	if (node->type == AstType::Else)
		return;

	AstCond *cond = static_cast<AstCond *>(node);
	std::string ln = "";
	
	//Build the left side of the comparison
	switch (cond->lval->type) {
		//Other variables
		case AstType::Id: {
			AstID *id = static_cast<AstID *>(cond->lval);
			Var v = vars[id->get_name()];
			
			ln = "ldr r3, [fp, #-";
			ln += std::to_string(v.stack_pos) + "]";
			sec_text.push_back(ln);
		} break;
		
		//TODO: Add the rest
	}
	
	//Build the right side of the comparison
	switch (cond->rval->type) {
		//Integers
		case AstType::Int: {
			AstInt *i = static_cast<AstInt *>(cond->rval);
			ln = "cmp r3, #" + std::to_string(i->get_val());
			sec_text.push_back(ln);
		}
		
		//TODO: Add the rest
	}
	
	//Build the operator
	std::string lbl = labels.top();
	labels.pop();
	
	switch (cond->get_op()) {
		case CondOp::Equals: sec_text.push_back("bne " + lbl); break;
		
		//TODO: Add the rest
	}
	
	sec_text.push_back("");
}
