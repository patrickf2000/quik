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
			
			writer << "\tmovss xmm1, [rbp-" << offset;
			writer << "]" << std::endl << std::endl;
		} break;
		
		//Variable index
		case ltac::Var: {
		
		} break;
	}
}
