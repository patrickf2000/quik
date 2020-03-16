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
			
			switch (var->d_type) {
				//Integers
				case DataType::Int: {
					writer << "\tmov eax, [rbp-" << var2->pos;
					writer << "]" << std::endl;
					
					writer << "\tmov DWORD PTR [rbp-";
					writer << var->pos << "], eax" << std::endl;
				} break;
				
				//128-bit vector types
				case DataType::Int128:
				case DataType::Float128: {
					writer << "\tmovups xmm0, [rbp-" << var2->pos;
					writer << "]" << std::endl;
					
					writer << "\tmovups [rbp-";
					writer << var->pos << "], xmm0" << std::endl;
				} break;
				
				//256-bit vector types
				case DataType::Int256:
				case DataType::Float256: {
					writer << "\tvmovups ymm0, [rbp-" << var2->pos;
					writer << "]" << std::endl;
					
					writer << "\tvmovups [rbp-";
					writer << var->pos << "], ymm0" << std::endl;
				} break;
			}
		} break;
		
		//Math expressions
		case ltac::Math: {
			switch (var->d_type) {
				case DataType::Int: build_int_math(var, src); break;
				case DataType::Float: build_float_math(var, src); break;
				case DataType::Int128:
				case DataType::Int256:
				case DataType::Float128:
				case DataType::Float256: build_vector_math(var, src); break;
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
void Asm_x64::build_vector_math(LtacVar *var, LtacNode *src) {
	auto math = static_cast<LtacMath *>(src);
	
	//Determine if we are using 256-bit registers
	bool islong = false;
	if (var->d_type == DataType::Int256 || var->d_type == DataType::Float256)
		islong = true;
	
	//Print the first value
	auto first = math->init_val;
	
	switch (first->type) {
		//Variables
		case ltac::Var: {
			auto id = static_cast<LtacVar *>(first);
			
			if (islong)
				writer << "\tvmovups ymm0, ";
			else
				writer << "\tmovups xmm0, ";
			
			writer << "[rbp-" << std::to_string(id->pos);
			writer << "]" << std::endl;
		} break;
		
		//TODO: Add rest (we support constants)
	}
	
	//Build the rest of the equation
	for (auto node : math->children) {
		auto math_op = static_cast<LtacMathOp *>(node);
		std::string ln = "\t";
		
		//Build the operator
		//Intel is a pain :)
		switch (var->d_type) {
			//Int 128
			case DataType::Int128: {
				switch (math_op->op) {
					case Operator::Add: ln += "phaddd xmm0, "; break;
					case Operator::PAdd: ln += "addps xmm0, "; break;
					case Operator::Sub: ln += "psubb xmm0, "; break;
					case Operator::Mul: ln += "pmulld xmm0, "; break;
				}
			} break;
			
			//Int 256
			case DataType::Int256: {
				switch (math_op->op) {
					case Operator::Add: ln += "vphaddd ymm0, ymm0, "; break;
					case Operator::PAdd: ln += "vpaddd ymm0, ymm0, "; break;
					case Operator::Sub: ln += "vpsubb ymm0, ymm0, "; break;
					case Operator::Mul: ln += "vpmulld ymm0, ymm0, "; break;
				}
			} break;
			
			//Float 128
			case DataType::Float128: {
				switch (math_op->op) {
					case Operator::Add: ln += "haddps xmm0, "; break;
					case Operator::PAdd: ln += "addps xmm0, "; break;
					case Operator::Sub: ln += "subps xmm0, "; break;
					case Operator::Mul: ln += "mulps xmm0, "; break;
					case Operator::Div: ln += "divps xmm0, "; break;
				}
			} break;
			
			//Float 256
			case DataType::Float256: {
				switch (math_op->op) {
					case Operator::Add: ln += "vhaddps ymm0, ymm0, "; break;
					case Operator::PAdd: ln += "vaddps ymm0, ymm0, "; break;
					case Operator::Sub: ln += "vsubps ymm0, ymm0, "; break;
					case Operator::Mul: ln += "vmulps ymm0, ymm0, "; break;
					case Operator::Div: ln += "vdivps ymm0, ymm0, "; break;
				}
			} break;
		}
		
		//Build the operand
		switch (math_op->operand->type) {
			//Other variables
			case ltac::Var: {
				auto id = static_cast<LtacVar *>(math_op->operand);
				ln += "[rbp-" + std::to_string(id->pos) + "]";
			} break;
			
			//The index of an array
			case ltac::ArrayAcc: {
				auto acc = static_cast<LtacArrayAcc *>(math_op->operand);
				auto child = acc->children[0];
				
				int pos = acc->stack_pos;
				int size = acc->type_size;
				
				switch (child->type) {
					//Integer access
					case ltac::Int: {
						auto li = static_cast<LtacInt *>(child);
						int offset = pos - (size * li->val);
						
						writer << "\tmovups xmm1, [rbp-" + std::to_string(offset);
						writer << "]" << std::endl;
					} break;
					
					//By-variable access
					case ltac::Var: {
						auto lv = static_cast<LtacVar *>(child);
						writer << "\tmov eax, DWORD PTR [rbp-" << std::to_string(lv->pos);
						writer << "]" << std::endl;
						writer << "\tcdqe" << std::endl;
						writer << "\tmovups xmm1, [rbp-" << std::to_string(pos);
						writer << "+rax*4]" << std::endl;
					} break;
				}
			
				if (islong)
					ln += "ymm1";
				else
					ln += "xmm1";
			} break;
			
			//TODO: Add rest
		}
		
		//Write out the line
		writer << ln << std::endl;
	}
	
	//Save results back to the variable
	std::string mem = "[rbp-" + std::to_string(var->pos) + "]";
	
	if (islong)
		writer << "\tvmovups " << mem << ", ymm0";
	else
		writer << "\tmovups " << mem << ", xmm0";
	
	writer << std::endl << std::endl;
}

