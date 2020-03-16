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
			switch (var->d_type) {
				case DataType::Int: build_int_math(var, src); break;
				case DataType::Float: build_float_math(var, src); break;
				case DataType::Int128:
				case DataType::Int256: build_vint_math(var, src); break;
			}
		} break;
		
		//Function calls
		case ltac::FuncCall: {
			build_func_call(src);
			
			writer << "\tmov DWORD PTR [rbp-";
			writer << var->pos << "], eax" << std::endl;
		} break;
		
		//Array access
		case ltac::ArrayAcc: {
			build_array_acc(src);
			
			switch (var->d_type) {
				//Ints
				case DataType::Int: {
					writer << "\tmov DWORD PTR [rbp-";
					writer << var->pos << "], ebx" << std::endl;
				} break;
				
				//Floats
				case DataType::Float: {
					writer << "\tmovss DWORD PTR [rbp-";
					writer << var->pos << "], xmm1" << std::endl;
				} break;
			}
			
			writer << std::endl;
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

//Builds floating-point math expressions
void Asm_x64::build_float_math(LtacVar *var, LtacNode *src) {
	auto math = static_cast<LtacMath *>(src);
	
	//Build the first value
	auto first = math->init_val;
	
	switch (first->type) {
		//An integer
		case ltac::Float: {
			auto i = static_cast<LtacFloat *>(first);
			writer << "\tmovss xmm0, [" << i->name << "]" << std::endl;
		} break;
		
		//A variable
		case ltac::Var: {
			auto id = static_cast<LtacVar *>(first);
			writer << "\tmovss xmm0, [rbp-" << std::to_string(id->pos);
			writer << "]" << std::endl;
		} break;
	}
	
	//Build the other parts
	for (auto node : math->children) {
		auto math_op = static_cast<LtacMathOp *>(node);
		
		//Build the operator
		switch (math_op->op) {
			case Operator::Add: writer << "\taddss xmm0, "; break;
			case Operator::Sub: writer << "\tsubss xmm0, "; break;
			case Operator::Mul: writer << "\tmulss xmm0, "; break;
			case Operator::Div: writer << "\tdivss xmm0, "; break;
		}
		
		//Build the operand
		switch (math_op->operand->type) {
			case ltac::Float: {
				auto i = static_cast<LtacFloat *>(math_op->operand);
				writer << "[" << i->name << "]" << std::endl;
			} break;
			
			case ltac::Var: {
				auto id = static_cast<LtacVar *>(math_op->operand);
				writer << "DWORD PTR ";
				writer << "[rbp-" << std::to_string(id->pos) << "]";
				writer << std::endl;
			} break;
		}
	}
	
	//Save the result back to the variable
	writer << "\tmovss DWORD PTR [rbp-" << std::to_string(var->pos);
	writer << "], xmm0" << std::endl << std::endl;
}

//Builds vector integer math
void Asm_x64::build_vint_math(LtacVar *var, LtacNode *src) {
	auto math = static_cast<LtacMath *>(src);
	
	//Print the first value
	auto first = math->init_val;
	
	switch (first->type) {
		//Variables
		case ltac::Var: {
			auto id = static_cast<LtacVar *>(first);
			writer << "\tmovups xmm1, [rbp-" << std::to_string(id->pos);
			writer << "]" << std::endl;
		} break;
		
		//TODO: Add rest (we support constants)
	}
	
	//Build the rest of the equation
	for (auto node : math->children) {
		auto math_op = static_cast<LtacMathOp *>(node);
		
		//Build the operator
		switch (math_op->op) {
			case Operator::Add: writer << "\tphaddd xmm1, "; break;
			
			//TODO: Add rest
		}
		
		//Build the operand
		switch (math_op->operand->type) {
			case ltac::Var: {
				auto id = static_cast<LtacVar *>(math_op->operand);
				writer << "[rbp-" << std::to_string(id->pos) << "]";
				writer << std::endl;
			} break;
			
			//TODO: Add rest
		}
	}
	
	//Save results back to the variable
	writer << "\tmovups [rbp-" << std::to_string(var->pos) << "], xmm1";
	writer << std::endl << std::endl;
}

