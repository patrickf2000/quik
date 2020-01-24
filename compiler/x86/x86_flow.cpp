//x86_flow
//Contains the functions for assembling conditionals,
// loops, and other flow control statements
#include "asmx86.hh"

//Generates assembly for a conditional statement
void Asm_x86::build_cond(AstNode *node) {
	AstCond *cond = dynamic_cast<AstCond *>(node);

	//Set up the labels
	std::string lbl = "L" + std::to_string(lbl_index);
	++lbl_index;
	labels.push(lbl);
	
	if (node->type != AstType::Else) {
		//Determine the register
		std::string c_reg = "eax";
		
		if (cond->rval->type == AstType::Id) {
			auto id = dynamic_cast<AstID *>(cond->rval);
			std::string name = id->get_name();
			Var v = current_scope->vars[name];
			
			if (v.type == DataType::Char)
				c_reg = "al";
		} else if (cond->rval->type == AstType::Char) {
			c_reg = "al";
		}
		
		std::string ln = "";
	
		//Position the lval
		if (cond->lval->type == AstType::FuncCall) {
			AstFuncCall *fc = static_cast<AstFuncCall *>(cond->lval);
			build_func_call(fc);
		} else {
			ln = "mov eax, " + type2asm(cond->lval);
			sec_text.push_back(ln);
		}
		
		//Position the rval
		if (cond->rval->type == AstType::FuncCall) {
			sec_text.push_back("mov ebx, eax");
			
			AstFuncCall *fc = static_cast<AstFuncCall *>(cond->rval);
			build_func_call(fc);
			
			sec_text.push_back("cmp eax, ebx");
		} else {
			ln = "cmp " + c_reg + ", " + type2asm(cond->rval);
			sec_text.push_back(ln);
		}
		
		//Comparison
		switch (cond->get_op()) {
			case CondOp::Equals: ln = "jne "; break;
			case CondOp::NotEquals: ln = "je "; break;
			case CondOp::Greater: ln = "jle "; break;
			case CondOp::GreaterEq: ln = "jle "; break;
			case CondOp::Less: ln = "jge "; break;
			case CondOp::LessEq: ln = "jge "; break;
		}
		
		ln += lbl;
	
		sec_text.push_back(ln);
		sec_text.push_back("");
	}
}

//Generates assembly for a while loop
void Asm_x86::build_while(AstNode *node) {
	//Set up the labels
	auto cmp_lbl = labels.top();
	labels.pop();
	
	auto lbl = labels.top();
	labels.pop();
	
	sec_text.push_back(cmp_lbl + ":");
	
	//While loops
	if (node->type == AstType::While) {
		AstCond *cond = dynamic_cast<AstCond *>(node);
			
		//Position the lval
		std::string ln = "mov eax, " + type2asm(cond->lval);
		sec_text.push_back(ln);
		
		//Position the rval
		ln = "cmp eax, " + type2asm(cond->rval);
		sec_text.push_back(ln);
		
		//Comparison
		switch (cond->get_op()) {
			case CondOp::Equals: ln = "je "; break;
			case CondOp::NotEquals: ln = "jne "; break;
			case CondOp::Greater: ln = "jg "; break;
			case CondOp::GreaterEq: ln = "jge "; break;
			case CondOp::Less: ln = "jl "; break;
			case CondOp::LessEq: ln = "jle "; break;
		}
		
		ln += lbl;

		sec_text.push_back(ln);
		
	//Loops
	} else if (node->type == AstType::Loop) {
		AstLoop *lp = static_cast<AstLoop *>(node);
		
		if (lp->param == nullptr) {
			sec_text.push_back("jmp " + lbl);
		} else {
			switch (lp->param->type) {
				case AstType::Int: {
					std::string dest = "[rbp-" + std::to_string(stack_pos) + "]";
					sec_text.push_back("add dword " + dest + ", 1");
					stack_pos += 4;
					
					AstInt *i = static_cast<AstInt *>(lp->param);
					sec_text.push_back("mov eax, " + std::to_string(i->get_val() + 1));
					
					std::string ln = "cmp eax, " + dest;
					sec_text.push_back(ln);
					
					ln = "jne " + lbl;
					sec_text.push_back(ln);
					
					ln = "mov dword " + dest + ", 0";
					sec_text.push_back(ln);
				} break;
			
				case AstType::Id: {
				
				} break;
			}
		}
	}
	
	sec_text.push_back("");
}

