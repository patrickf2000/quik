#include "asm_x64.hh"

//Builds an array
void Asm_x64::build_array(LtacNode *node) {
	auto array = static_cast<LtacArray *>(node);
	
	int pos = array->stack_pos;
	int size = array->type_size;
	
	for (auto child : array->children) {
		std::string addr = "[rbp-" + std::to_string(pos) + "]";
		pos -= size;
	
		switch (child->type) {
			//Integers
			case ltac::Int: {
				auto i = static_cast<LtacInt *>(child);
			
				writer << "\tmov DWORD PTR " << addr << ", ";
				writer << i->val << std::endl;
			} break;
			
			//Floating point numbers
			case ltac::Float: {
				auto f = static_cast<LtacFloat *>(child);
				
				writer << "\tmovss xmm0, DWORD PTR ";
				writer << f->name << "[rip]" << std::endl;
				writer << "\tmovss DWORD PTR " << addr << ", ";
				writer << "xmm0" << std::endl;
			} break;
			
			//TODO: Add the rest
		}
	}
	
	writer << std::endl;
}

//Builds an array access
void Asm_x64::build_array_acc(LtacNode *node) {
	auto acc = static_cast<LtacArrayAcc *>(node);
	auto child = acc->children[0];
	
	int pos = acc->stack_pos;
	int size = acc->type_size;
	
	switch (child->type) {
		//Integer index
		case ltac::Int: {
			auto li = static_cast<LtacInt *>(child);
			int offset = pos - (size * li->val);
			
			switch (acc->d_type) {
				//Integer arrays
				case DataType::Int: {
					writer << "\tmov ebx, [rbp-" << offset;
					writer << "]" << std::endl;
				} break;
				
				//Float arrays
				case DataType::Float: {
					writer << "\tmovss xmm1, [rbp-" << offset;
					writer << "]" << std::endl;
				} break;
			}
		} break;
		
		//Variable index
		case ltac::Var: {
			auto lv = static_cast<LtacVar *>(child);
			
			switch (acc->d_type) {
				//Integer arrays
				case DataType::Int: 
				case DataType::Int128:
				case DataType::Int256: {
					writer << "\tmov eax, DWORD PTR [rbp-" << lv->pos;
					writer << "]" << std::endl;
					writer << "\tcdqe" << std::endl;
					
					writer << "\tmov ebx, DWORD PTR [rbp-" << pos << "+rax*";
					writer << size << "]" << std::endl;
				} break;
				
				//Float arrays
				case DataType::Float: 
				case DataType::Float128:
				case DataType::Float256: {
					writer << "\tmov eax, DWORD PTR [rbp-" << lv->pos;
					writer << "]" << std::endl;
					writer << "\tcdqe" << std::endl;
					
					writer << "\tmovss xmm1, DWORD PTR [rbp-" << pos << "+rax*";
					writer << size << "]" << std::endl;
				} break;
			}
		} break;
	}
}
