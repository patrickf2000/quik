#include "asm_x64.hh"

//Integer comparison
void Asm_x64::build_icmp(LtacNode *node) {
	auto cmp = static_cast<LtacCmp *>(node);
	
	//Build the left value
	switch (cmp->lval->type) {
		case ltac::Int: {
			auto i = static_cast<LtacInt *>(cmp->lval);
			writer << "\tmov eax, " << std::to_string(i->val);
			writer << std::endl;
		} break;
		
		case ltac::Var: {
			auto var = static_cast<LtacVar *>(cmp->lval);
			writer << "\tmov eax, DWORD PTR [rbp-";
			writer << std::to_string(var->pos) << "]";
			writer << std::endl;
		} break;
		
		case ltac::FuncCall: {
			build_func_call(cmp->lval);
		} break;
	}
	
	//Compare with the right value
	std::string ln = "\tcmp eax, ";
	
	switch (cmp->rval->type) {
		case ltac::Int: {
			auto i = static_cast<LtacInt *>(cmp->rval);
			ln += std::to_string(i->val) + "\n";
		} break;
		
		case ltac::Var: {
			auto var = static_cast<LtacVar *>(cmp->rval);
			ln += "[rbp-" + std::to_string(var->pos) + "]\n";
		} break;
		
		case ltac::FuncCall: {
			ln = "\tmov ebx, eax\n" + ln;
			ln += "ebx\n";
			build_func_call(cmp->rval);
		} break;
	}
	
	writer << ln;
}

//Build jumps (all kinds)
void Asm_x64::build_jmp(LtacNode *node) {
	auto jmp = static_cast<LtacJmp *>(node);
	
	switch (jmp->op) {
		case Operator::None: writer << "\tjmp "; break;
		case Operator::Equal: writer << "\tje "; break;
		case Operator::NotEqual: writer << "\tjne "; break;
		case Operator::Greater: writer << "\tjg "; break;
		case Operator::Less: writer << "\tjl "; break;
		case Operator::GreaterEq: writer << "\tjge "; break;
		case Operator::LessEq: writer << "\tjle "; break;
	}
	
	writer << jmp->dest << std::endl;
	writer << std::endl;
}
