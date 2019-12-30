//x86_funcs
//Contains the functions for assembling function-related
// declarations, calls, and controls
#include "asmx86.hh"

//Assembles a function declaration
void Asm_x86::build_function(AstNode *node) {
	AstFuncDec *fd = dynamic_cast<AstFuncDec *>(node);
	
	std::string ln = fd->get_name();
	extern_data.push_back("global " + ln);
	
	if (ln == "main") {
		in_main = true;
	} else {
		in_main = false;
	}
	
	ln += ":";
	sec_text.push_back(ln);
	
	//Setup the stack
	//Push the base pointer, and set the base pointer
	// as the current stack pointer
	sec_text.push_back("push ebp");
	sec_text.push_back("mov ebp, esp");
	
	//TODO: Add some logic instead of randomly assigning some large number
	if (in_main)
		sec_text.push_back("sub esp, 48");
	
	sec_text.push_back("");
	
	//Build the arguments
	for (auto v : fd->args) {
		//Determine the stack position
		switch (v.type) {
			case DataType::Byte:
			case DataType::Char: stack_pos += 1; break;
			case DataType::Short: stack_pos += 2; break;
			case DataType::Bool:
			case DataType::Int:
			case DataType::Float: stack_pos += 4; break;
			case DataType::Str: stack_pos += 8; break;
		}
		
		v.stack_pos = stack_pos;
		vars[v.name] = v;
		
		//Move in the value
		sec_text.push_back("mov eax, [esp-" + std::to_string(stack_pos) + "]");
		sec_text.push_back("mov [ebp-" + std::to_string(stack_pos) + "], eax");
		sec_text.push_back("");
	}
}

//Assembles an external function
void Asm_x86::build_extern_func(AstNode *node) {
	AstExternFunc *fd = dynamic_cast<AstExternFunc *>(node);
	std::string ln = "extern " + fd->get_name();
	
	extern_data.push_back(ln);
}

//Assembles a function call
void Asm_x86::build_func_call(AstFuncCall *fc) {
	for (int i = fc->children.size()-1; i>=0; i--) {
		auto node = fc->children.at(i);
	
		switch (node->type) {
			//An ID-> Means we have a variable
			case AstType::Id: {
				AstID *id = dynamic_cast<AstID *>(node);
				Var v = vars[id->get_name()];
				std::string ln = "";
			
				switch (v.type) {
					case DataType::Byte:
					case DataType::Char:
					case DataType::Short:
					case DataType::Bool:
					case DataType::Int: {
						ln = "push dword [ebp-";
						ln += std::to_string(v.stack_pos) + "]";
					} break;
					case DataType::Long:
					case DataType::Float: {
							ln = "fld qword [" + v.name + "]";
					} break;
					case DataType::Str: {
						if (v.is_param)
							ln = "push dword [" + v.name + "]";
						else
							ln = "push dword " + v.name;
					} break;
				}
				
				sec_text.push_back(ln);
			} break;
			
			//An Int
			case AstType::Int: {
				AstInt *no = dynamic_cast<AstInt *>(node);
				sec_text.push_back("push " + std::to_string(no->get_val()));
			} break;
			
			//A float
			case AstType::Float: type2flt(node); break;
			
			//A string
			case AstType::Str: {
				AstString *str = dynamic_cast<AstString *>(node);
				
				std::string name = "STR_" + std::to_string(str_index);
				sec_data.push_back(name + " db \"" + str->get_val() + "\",0");
				++str_index;
				
				sec_text.push_back("push dword " + name);
			} break;
		}
	}

	sec_text.push_back("call " + fc->get_name());
	
	for (int i = 0; i<fc->children.size(); i++) {
		sec_text.push_back("add esp, 4");
	}
	
	sec_text.push_back("");	
}

//Builds the return statements
void Asm_x86::build_ret(AstNode *node) {
	stack_pos = 0;
	vars.clear();

	if (node->children.size() == 1) {
		auto child = node->children.at(0);
		if (child->type == AstType::Float) {
			type2flt(child);
		} else if (child->type == AstType::Id) {
			AstID *id = dynamic_cast<AstID *>(child);
			Var v = current_scope->vars[id->get_name()];
			
			if (v.type == DataType::Float)
				type2flt(child);
			else
				sec_text.push_back("mov eax, " + type2asm(child));
		} else {
			sec_text.push_back("mov eax, " + type2asm(child));
		}
	}
	
	if (in_main)
		sec_text.push_back("leave");
	else
		sec_text.push_back("pop ebp");
		
	sec_text.push_back("ret");
	sec_text.push_back("");
}

