#include "asm_x64.hh"

//Build a variable declaration
void Asm_x64::build_var(LtacNode *node) {
	auto var = static_cast<LtacVar *>(node);
	auto src = node->children[0];
	
	switch (src->type) {
		//Character
		case ltac::Byte: {
			writer << "\tmov BYTE PTR [rbp-";
			writer << var->pos << "], ";
			
			auto li = static_cast<LtacInt *>(src);
			writer << li->val << std::endl;
		} break;
	
		//Integers
		case ltac::Int: {
			writer << "\tmov DWORD PTR [rbp-";
			writer << var->pos << "], ";
			
			auto li = static_cast<LtacInt *>(src);
			writer << li->val << std::endl;
		} break;
		
		//Floats
		case ltac::Float: {
			auto lf = static_cast<LtacFloat *>(src);
		
			writer << "\tmovss xmm0, DWORD PTR ";
			writer << lf->name << "[rip]" << std::endl;
			
			writer << "\tmovss DWORD PTR [rbp-";
			writer << var->pos << "], xmm0" << std::endl;
		} break;
		
		//Strings
		case ltac::String: {
			writer << "\tmov QWORD PTR [rbp-";
			writer << var->pos << "], ";
			writer << "OFFSET FLAT:";
			
			auto lstr = static_cast<LtacString *>(src);
			writer << lstr->name << std::endl;
		} break;
		
		//Variables
		case ltac::Var: {
			auto var2 = static_cast<LtacVar *>(src);
			writer << std::endl;
			writer << "\tmov eax, [rbp-" << var2->pos;
			writer << "]" << std::endl;
			
			writer << "\tmov DWORD PTR [rbp-";
			writer << var->pos << "], eax" << std::endl;
		} break;
		
		//Math expressions
		case ltac::Math: {
			if (var->d_type == DataType::Int)
				build_int_math(var, src);
		} break;
		
		//Function calls
		case ltac::FuncCall: {
			build_func_call(src);
			
			writer << "\tmov DWORD PTR [rbp-";
			writer << var->pos << "], eax" << std::endl;
		} break;
	}
}

//Builds integer math expressions
void Asm_x64::build_int_math(LtacVar *var, LtacNode *src) {
	auto math = static_cast<LtacMath *>(src);
	
	//Build the first value
	auto first = math->init_val;
	
	switch (first->type) {
		//An integer
		case ltac::Int: {
			auto i = static_cast<LtacInt *>(first);
			writer << "\tmov eax, " << std::to_string(i->val) << std::endl;
		} break;
		
		//A variable
		case ltac::Var: {
			auto id = static_cast<LtacVar *>(first);
			writer << "\tmov eax, [rbp-" << std::to_string(id->pos);
			writer << "]" << std::endl;
		} break;
	}
	
	//Build the other parts
	for (auto node : math->children) {
		auto math_op = static_cast<LtacMathOp *>(node);
		bool is_mod = false;
		
		//Build the operator
		switch (math_op->op) {
			case Operator::Add: writer << "\tadd eax, "; break;
			case Operator::Sub: writer << "\tsub eax, "; break;
			case Operator::Mul: writer << "\timul eax, "; break;
			case Operator::Div: {
				writer << "\tcdq" << std::endl;
				writer << "\tidiv "; 
			} break;
			case Operator::Mod: {
				writer << "\tcdq" << std::endl;
				writer << "\tidiv ";
				is_mod = true;
			} break;
		}
		
		//Build the operand
		switch (math_op->operand->type) {
			case ltac::Int: {
				auto i = static_cast<LtacInt *>(math_op->operand);
				writer << std::to_string(i->val) << std::endl;
			} break;
			
			case ltac::Var: {
				auto id = static_cast<LtacVar *>(math_op->operand);
				writer << "DWORD PTR ";
				writer << "[rbp-" << std::to_string(id->pos) << "]";
				writer << std::endl;
			} break;
		}
		
		if (is_mod)
			writer << "\tmov eax, edx" << std::endl;
	}
	
	//Save the result back to the variable
	writer << "\tmov DWORD PTR [rbp-" << std::to_string(var->pos);
	writer << "], eax" << std::endl << std::endl;
}

