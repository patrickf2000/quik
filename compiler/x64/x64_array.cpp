#include "asm_x64.hh"

//Builds an array
void Asm_x64::build_array(LtacNode *node) {
	auto array = static_cast<LtacArray *>(node);
	
	int pos = array->stack_pos;
	int size = array->size;
	
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
			
			//TODO: Add the rest
		}
	}
	
	writer << std::endl;
}
