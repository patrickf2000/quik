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
	
	//While loops
	if (node->type == AstType::While) {
		AstCond *cond = dynamic_cast<AstCond *>(node);
		sec_text.push_back(cmp_lbl + ":");
			
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
			std::string dest = lp->i_var;
		
			//Increment the value
			sec_text.push_back("add dword " + dest + ", 1");
					
			//Insert the comparison label
			sec_text.push_back(cmp_lbl + ":");
					
			switch (lp->param->type) {
				//Hard-coded values
				case AstType::Int: {
					AstInt *i = static_cast<AstInt *>(lp->param);
					sec_text.push_back("mov eax, " + std::to_string(i->get_val()));
				} break;
			
				//Variables containing the value
				case AstType::Id: {
					AstID *id = static_cast<AstID *>(lp->param);
					Var v2 = vars[id->get_name()];
					std::string p_dest = "[rbp-" + std::to_string(v2.stack_pos) + "]";
					
					sec_text.push_back("mov eax, " + p_dest);
				} break;
			}
			
			//Generate the comparison
			std::string ln = "cmp eax, " + dest;
			sec_text.push_back(ln);
				
			ln = "jne " + lbl;
			sec_text.push_back(ln);
		}
	}
	
	sec_text.push_back("");
}

