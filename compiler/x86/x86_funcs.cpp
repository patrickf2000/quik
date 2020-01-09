//x86_funcs
//Contains the functions for assembling function-related
// declarations, calls, and controls
#include "asmx86.hh"

std::string call_regs[] = {
	"rdi",
	"rsi",
	"rdx",
	"rcx",
	"r8",
	"r9"
};

std::string call_regs32[] = {
	"edi",
	"esi",
	"edx",
	"ecx",
	"r8",
	"r9"
};

std::string call_flt_regs[] = {
	"xmm0",
	"xmm1",
	"xmm2",
	"xmm3",
	"xmm4",
	"xmm5",
	"xmm6",
	"xmm7"
};

//Assembles a function declaration
void Asm_x86::build_function(AstNode *node) {
	AstFuncDec *fd = static_cast<AstFuncDec *>(node);
	
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
	
	//Do the architecture-specific stuff
	if (x64)
		build_func_x64(fd);
	else
		build_func_i386(fd);
}

//Build a function declaration (x86 32-bit)
void Asm_x86::build_func_i386(AstFuncDec *fd) {
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
}

//Build a function declaration (x86 64-bit)
void Asm_x86::build_func_x64(AstFuncDec *fd) {
	//Setup the stack
	sec_text.push_back("push rbp");
	sec_text.push_back("mov rbp, rsp");
	
	//TODO: Add some logic instead of randomly assigning some large number
	sec_text.push_back("sub rsp, 56");
	
	sec_text.push_back("");
	
	//Build the arguments
	stack_pos = 8;
	if (fd->args.size() == 0)
		stack_pos = 4;
		
	int call_index = 0;
	
	for (auto v : fd->args) {
		v.stack_pos = stack_pos;
		vars[v.name] = v;
		
		//Determine the proper register
		std::string reg = "eax";
		
		if (v.type == DataType::Char) {
			reg = "al";
		}
		
		//Move in the value
		if (v.is_array) {
			sec_text.push_back("mov rax, " + call_regs[call_index]);
			reg = "rax";
		} else {
			sec_text.push_back("mov eax, " + call_regs32[call_index]);
		}
		
		sec_text.push_back("mov [rbp-" + std::to_string(stack_pos) + "], " + reg);
		sec_text.push_back("");
		
		//Determine the stack position
		switch (v.type) {
			case DataType::Byte:
			case DataType::Char: stack_pos += 1; break;
			case DataType::Short: stack_pos += 2; break;
			case DataType::Bool:
			case DataType::Int:
			case DataType::Str:
			case DataType::Float: stack_pos += 4; break;
		}
		
		//Increment the call index
		++call_index;
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
	if (x64)
		build_func_call_x64(fc);
	else
		build_func_call_i386(fc);
}

//Build a 32-bit function call
void Asm_x86::build_func_call_i386(AstFuncCall *fc) {
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
							std::string ln1 = "lea eax, [ebp-";
							ln1 += std::to_string(v.stack_pos) + "]";
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
			//TODO: Add me
			case AstType::Float: break;
			
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

//Build a 64-bit function call
void Asm_x86::build_func_call_x64(AstFuncCall *fc) {
	int call_index = 0;
	int flt_call_index = 0;
	
	for (auto node : fc->children) {
		bool flt_arg = false;
		std::string call_ln = "mov " + call_regs32[call_index] + ", ";
		
		switch (node->type) {
			//TODO: Add the reset
			
			//An ID
			case AstType::Id: {
				AstID *id = static_cast<AstID *>(node);
				Var v = vars[id->get_name()];
				
				call_ln = "mov " + call_regs32[call_index] + ", ";
				
				//TODO: Add the rest
				switch (v.type) {
					//Char variables
					case DataType::Char: {
						std::string ln = "movsx eax, byte [rbp-";
						ln += std::to_string(v.stack_pos) + "]";
						
						sec_text.push_back(ln);
						sec_text.push_back(call_ln + "eax");
					} break;
					
					//Integer variables and arrays
					case DataType::Int: {
						if (v.is_array) {
							std::string ln1 = "lea rax, [rbp-";
							ln1 += std::to_string(v.stack_pos) + "]";
							sec_text.push_back(ln1);
							
							call_ln = "mov " + call_regs[call_index] + ", ";
							sec_text.push_back(call_ln + "rax");
						} else {
							std::string ln = "mov eax, [rbp-";
							ln += std::to_string(v.stack_pos) + "]";
							
							sec_text.push_back(ln);
							sec_text.push_back(call_ln + "eax");
						}
					} break;
					
					//Floating-point variables
					case DataType::Float: {
						flt_arg = true;
						
						call_ln = "movq " + call_flt_regs[flt_call_index] + ", ";
						call_ln += "[rbp-" + std::to_string(v.stack_pos) + "]";
						
						sec_text.push_back(call_ln);
					} break;
					
					//String variables
					case DataType::Str: {
						call_ln += "[rbp-" + std::to_string(v.stack_pos) + "]";
						sec_text.push_back(call_ln);
					} break;
				}
			} break;
			
			//An integer
			case AstType::Int: {
				AstInt *i = static_cast<AstInt *>(node);
				auto val = std::to_string(i->get_val());
				sec_text.push_back(call_ln + val);
			} break;
			
			//A float
			case AstType::Float: {
				flt_arg = true;
				auto name = build_float(node);
						
				call_ln = "movq " + call_flt_regs[flt_call_index] + ", ";
				call_ln += "[" + name + "]";
				
				sec_text.push_back(call_ln);
			} break;
			
			//A string
			case AstType::Str: {
				auto name = build_string(node);
				sec_text.push_back(call_ln + name);
			} break;
		}
		
		if (flt_arg)
			++flt_call_index;
		else
			++call_index;
	}
	
	sec_text.push_back("call " + fc->get_name());
	sec_text.push_back("");
}

//Builds the return statements
//TODO: Fix the float stuff, its probably wrong
void Asm_x86::build_ret(AstNode *node) {
	if (node->children.size() == 1) {
		auto child = node->children.at(0);
		if (child->type == AstType::Float) {
			sec_text.push_back("mov xmm0, 0");
		} else if (child->type == AstType::Id) {
			AstID *id = dynamic_cast<AstID *>(child);
			Var v = current_scope->vars[id->get_name()];
			
			if (v.type == DataType::Float)
				sec_text.push_back("mov xmm0, 0");
			else
				sec_text.push_back("mov eax, " + type2asm(child));
		} else {
			sec_text.push_back("mov eax, " + type2asm(child));
		}
	}
	
	stack_pos = 0;
	vars.clear();
	
	if (x64) {
		sec_text.push_back("leave");
	} else {
		if (in_main) {
			sec_text.push_back("leave");
		} else {
			sec_text.push_back("mov esp, ebp");
			sec_text.push_back("pop ebp");
		}
	}
		
	sec_text.push_back("ret");
	sec_text.push_back("");
}

