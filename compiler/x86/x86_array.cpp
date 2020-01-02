//x86_array
//Contains the functions for assembling arrays
#include "asmx86.hh"

//Builds an array declaration
void Asm_x86::build_arr_dec(AstNode *node) {
	AstArrayDec *ard = static_cast<AstArrayDec *>(node);
	
	Var v;
	v.name = ard->get_name();
	v.type = ard->get_type();
	v.stack_pos = stack_pos;
	v.size = ard->children.size();
	v.is_array = true;
	v.is_param = false;
	vars[ard->get_name()] = v;
	
	//Determine the stack position and type
	int size = 1;
	int pos = 0;
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
	
	pos = size * v.size;
	stack_pos = pos;
	
	//Add the initial values
	for (auto child : ard->children) {
		auto ln = "mov " + prefix + " [ebp-";
		ln += std::to_string(pos) + "], ";
		
		pos -= size;
	
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
	
	Var v = vars[acc->get_name()];
	index = v.stack_pos;	
	
	if (v.type == DataType::Str)
		size = 1;
	
	//Calculate the index
	//For ints: 4 * (element index)
	//TODO: Modify not to only use ints
	auto i_child = acc->children.at(0);
	std::string ln = "mov eax, [ebp-";
	
	//If the array is a pointer, we need to handle a little differently
	if (v.is_param) {
		ln += std::to_string(index) + "]";
		sec_text.push_back(ln);
		
		switch (i_child->type) {
			case AstType::Int: {
				AstInt *i = dynamic_cast<AstInt *>(i_child);
				int i2 = i->get_val() * size;
				
				ln = "mov eax, [eax+" + std::to_string(i2) + "]";
			} break;
			
			case AstType::Id: {
				AstID *id = dynamic_cast<AstID *>(i_child);
				
				std::string ln2 = "mov ebx, [ebp-";
				ln2 += std::to_string(vars[id->get_name()].stack_pos);
				ln2 += "]";
				
				sec_text.push_back(ln2);
				
				std::string mul_line = "imul ebx, ";
				mul_line += std::to_string(size);
				sec_text.push_back(mul_line);
				
				ln = "mov eax, [eax+ebx]";
			} break;
		}
		
	//Otherwise normal
	} else {
		switch (i_child->type) {
			case AstType::Int: {
				AstInt *i = dynamic_cast<AstInt *>(i_child);
				int i2 = index + (i->get_val() * size);
				ln += std::to_string(i2);
			} break;
			
			case AstType::Id: {
				AstID *id = dynamic_cast<AstID *>(i_child);
				
				std::string ln2 = "mov ebx, [ebp-";
				ln2 += std::to_string(vars[id->get_name()].stack_pos);
				ln2 += "]";
				
				sec_text.push_back(ln2);
				
				std::string mul_line = "imul ebx, ";
				mul_line += std::to_string(size);
				sec_text.push_back(mul_line);
				
				ln += std::to_string((size*v.size));
				ln += "+ebx";
			} break;
		}
		
		//Translate
		ln += "]";
	}
	
	sec_text.push_back(ln);
}

