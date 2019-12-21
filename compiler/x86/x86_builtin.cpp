//x86_builtin
//Contains the functions for assembling builtins (system-specific calls)
#include "asmx86.hh"

//Assembles a println call
void Asm_x86::build_println(AstFuncCall *fc) {
	//We wrap this as a conditional in case this built-in is never called
	if (!use_printf) {
		//Push the info we need back
		extern_data.push_back("extern printf");

		sec_data.push_back("int_fmt db \"%d\",10,0");
		sec_data.push_back("str_fmt db \"%s\",10,0");
		sec_data.push_back("flt_fmt db \"%f\",10,0");
		
		use_printf = true;
	}
	
	//Add the code
	for (auto node : fc->children) {
		bool was_flt = false;
	
		//Align the call stack
		if (!in_main)
			sec_text.push_back("push eax");
		
		switch (node->type) {
			//Hard-coded string
			case AstType::Str: {
				AstString *str = dynamic_cast<AstString *>(node);
			
				std::string name = "STR_" + std::to_string(str_index);
				sec_data.push_back(name + " db \"" + str->get_val() + "\",0");
				++str_index;
				
				sec_text.push_back("push dword " + name);
				sec_text.push_back("push dword str_fmt");
				sec_text.push_back("call printf");
			} break;
			
			//Variable
			case AstType::Id: {
				AstID *id = dynamic_cast<AstID *>(node);
				Var v = current_scope->vars[id->get_name()];
				
				std::string name = v.name;
				std::string p2 = "";
				std::string fmt = "";
				
				switch (v.type) {
					case DataType::Byte:
					case DataType::Char:
					case DataType::Short:
					case DataType::Bool:
					case DataType::Int: {
							name = "[" + v.name + "]";
							fmt = "int_fmt"; 
						} break;
					case DataType::Long:
					case DataType::Float: {
						name = "[" + v.name + "+4]";
						p2 = "[" + v.name + "]";
						fmt = "flt_fmt";
						was_flt = true;
					} break;
					case DataType::Str:	fmt = "str_fmt";		
				}
				
				sec_text.push_back("push dword " + name);
				if (p2 != "")
					sec_text.push_back("push dword " + p2);
					
				sec_text.push_back("push dword " + fmt);
				sec_text.push_back("call printf");
			} break;
		}
		
		//Finish aligning the call stack
		if (!in_main) {
			if (was_flt)
				sec_text.push_back("add esp, 16");
			else
				sec_text.push_back("add esp, 12");
		}
		
		sec_text.push_back("");
	}
}

