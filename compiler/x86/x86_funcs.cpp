//x86_funcs
//Contains the functions for assembling function-related
// declarations, calls, and controls
#include "asmx86.hh"

//Assembles a function declaration
void Asm_x86::build_function(AstNode *node) {
	AstFuncDec *fd = dynamic_cast<AstFuncDec *>(node);
	
	std::string ln = fd->get_name();
	std::string global_ln = "global " + ln;
	
	if (config.build_lib)
		global_ln += ":function";
	
	extern_data.push_back(global_ln);
	
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
	//if (in_main)
		sec_text.push_back("sub esp, 48");
	
	sec_text.push_back("");
	
	//Build the arguments
	stack_pos = 4;
	int v_pos = 8;
	
	for (auto v : fd->args) {
		v.stack_pos = stack_pos;
		vars[v.name] = v;
		
		//Determine the proper register
		std::string reg = "eax";
		
		if (v.type == DataType::Char) {
			reg = "al";
		}
		
		//Move in the value
		sec_text.push_back("mov eax, [ebp+" + std::to_string(v_pos) + "]");
		sec_text.push_back("mov [ebp-" + std::to_string(stack_pos) + "], " + reg);
		sec_text.push_back("");
		
		//Determine the stack position
		switch (v.type) {
			case DataType::Byte:
			case DataType::Char: {
				v_pos += 1; 
				stack_pos += 1;
			} break;
			
			case DataType::Short: {
				v_pos += 2; 
				stack_pos += 2; 
			} break;
			
			case DataType::Bool:
			case DataType::Int:
			case DataType::Str:
			case DataType::Float: {
				v_pos += 4; 
				stack_pos += 4;
			} break;
		}
	}
	
	//stack_pos = 0;
}

//Assembles an external function
void Asm_x86::build_extern_func(AstNode *node) {
	AstExternFunc *fd = dynamic_cast<AstExternFunc *>(node);
	std::string ln = "extern " + fd->get_name();
	
	extern_data.push_back(ln);
}

//Assembles a function call
void Asm_x86::build_func_call(AstFuncCall *fc) {
	int size = fc->children.size();

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
						if (v.is_array) {
							int top = (4 * v.size);
							
							std::string ln1 = "lea eax, [ebp-";
							ln1 += std::to_string(top) + "]";
							sec_text.push_back(ln1);
							
							ln = "push eax";
						} else {
							ln = "push dword [ebp-";
							ln += std::to_string(v.stack_pos) + "]";
						}
					} break;
					case DataType::Long:
					case DataType::Float: {
							ln = "fld qword [" + v.name + "]";
					} break;
					case DataType::Str: {
						ln = "push dword [ebp-";
						ln += std::to_string(v.stack_pos) + "]";
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
				auto name = build_string(node);
				sec_text.push_back("push dword " + name);
			} break;
		}
	}

	sec_text.push_back("call " + fc->get_name());
	
	for (int i = 0; i<size; i++) {
		sec_text.push_back("add esp, 4");
	}
	
	sec_text.push_back("");	
}

//Builds the return statements
void Asm_x86::build_ret(AstNode *node) {
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
	
	stack_pos = 0;
	vars.clear();
	
	if (in_main) {
		sec_text.push_back("leave");
	} else {
		sec_text.push_back("mov esp, ebp");
		sec_text.push_back("pop ebp");
	}
		
	sec_text.push_back("ret");
	sec_text.push_back("");
}

