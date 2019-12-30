//x86_array
//Contains the functions for assembling arrays
#include "asmx86.hh"

//Builds an array declaration
void Asm_x86::build_arr_dec(AstNode *node) {
	AstArrayDec *ard = static_cast<AstArrayDec *>(node);
	
	Var v;
	v.name = ard->get_name();
	v.type = ard->get_type();
	vars[ard->get_name()] = v;
	
	//Determine the stack position and type
	int size = 1;
	std::string prefix = "byte";
	
	switch (v.type) {
		case DataType::Byte:
		case DataType::Char: {
				size = 1;
			} break;
			
		case DataType::Short: {
				size = 2;
				prefix = "word";
			} break;
			
		case DataType::Bool:
		case DataType::Int:
		case DataType::Float: {
				size = 4;
				prefix = "dword";
			} break;
			
		case DataType::Str: {
				size = 8;
				prefix = "qword";
			} break;
	}
	
	//Add the initial values
	for (auto child : ard->children) {
		auto ln = "mov " + prefix + " [ebp-";
		ln += std::to_string(stack_pos) + "], ";
		
		stack_pos += size;
	
		switch (child->type) {
			case AstType::Int: {
				AstInt *i = static_cast<AstInt *>(child);
				ln += std::to_string(i->get_val());
			} break;
		}
		
		sec_text.push_back(ln);
	}
	
	sec_text.push_back("");
}

//Generates assembly for an array access statement
void Asm_x86::build_arr_access(AstNode *node) {
	AstArrayAcc *acc = dynamic_cast<AstArrayAcc *>(node);
	int index = 0;
	int size = 4;
	
	Var v = current_scope->vars[acc->get_name()];
	if (v.type == DataType::Str)
		size = 1;
	
	//Calculate the index
	//For ints: 4 * (element index)
	//TODO: Modify not to only use ints
	auto i_child = acc->children.at(0);
	std::string ln = "mov eax, [";
	ln += acc->get_name() + "+";
	
	switch (i_child->type) {
		case AstType::Int: {
			AstInt *i = dynamic_cast<AstInt *>(i_child);
			index = i->get_val();
			index *= size;
			ln += std::to_string(index);
		} break;
		
		case AstType::Id: {
			AstID *id = dynamic_cast<AstID *>(i_child);
			
			sec_text.push_back("mov ebx, [" + id->get_name() + "]");
			
			std::string mul_line = "imul ebx, ";
			mul_line += std::to_string(size);
			sec_text.push_back(mul_line);
			
			ln += "ebx";
		} break;
	}
	
	//Translate
	ln += "]";
	
	sec_text.push_back(ln);
	sec_text.push_back("");
}

