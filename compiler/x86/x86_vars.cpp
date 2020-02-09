//x86_vars
//Contains the functions for assembling variable-specific stuff
#include "asmx86.hh"

//Assembles a variable declaration
void Asm_x86::build_var_dec(AstNode *node) {
	AstVarDec *vd = static_cast<AstVarDec *>(node);
	Var v = current_scope->vars[vd->get_name()];
	v.is_array = false;
	
	//Determine the stack position
	switch (v.type) {
		case DataType::Byte:
		case DataType::Char: stack_pos += 1; break;
		case DataType::Short: stack_pos += 2; break;
		case DataType::Bool:
		case DataType::Int:
		case DataType::Float:
		case DataType::Str: stack_pos += 4; break;
		case DataType::Int64: 
		case DataType::Float64: {
			stack_pos += 8;
			v.size = 2;
		} break;
		case DataType::Int128:
		case DataType::Float128: {
			stack_pos += 16;
			v.size = 4;
		} break;
		case DataType::Int256:
		case DataType::Float256: {
			stack_pos += 32;
			v.size = 8;
		} break;
	}
	
	v.stack_pos = stack_pos;
	vars[vd->get_name()] = v;
	
	build_var_assign(node);
}

//Assigns the eax register to a variable
void Asm_x86::assign_ax(std::string dest, Var v) {
	std::string ln = "mov " + asm_type(v) + " ";
	ln += dest + ", ";
	
	if (v.type == DataType::Char)
		ln += "al";
	else
		ln += "eax";
		
	sec_text.push_back(ln);
	sec_text.push_back("");
}

//Builds a variable assignment
void Asm_x86::build_var_assign(AstNode *node) {
	AstVarDec *vd = static_cast<AstVarDec *>(node);
	Var v = vars[vd->get_name()];
	
	if (vd->get_type() == DataType::Float) {
		build_flt_assign(node);
		return;
	} else if (vd->get_type() == DataType::Int64
		|| vd->get_type() == DataType::Float64 
		|| vd->get_type() == DataType::Int128
		|| vd->get_type() == DataType::Float128
		|| vd->get_type() == DataType::Int256
		|| vd->get_type() == DataType::Float256) {
		build_floatex_assign(node);
		return;	
	}
	
	auto child = vd->children.at(0);
	
	//Build the first variable
	std::string dest_var = "[" + get_reg("bp") + "-";
	dest_var += std::to_string(v.stack_pos) + "]";
	std::string ln = "";
	
	//Array assignments
	if (node->type == AstType::ArrayAssign) {
		dest_var = build_arr_assign(node, v);
		
	//Structure assignments
	} else if (node->type == AstType::StructMod) {
		dest_var = build_struct_mod(node);
		v = get_struct_child(node);
	}
	
	//Increments
	if (child->type == AstType::Inc) {
		ln = "add dword " + dest_var + ", 1";
		sec_text.push_back(ln);
		sec_text.push_back("");
		
	//Other variables
	} else if (child->type == AstType::Id) {
		ln = "mov eax, " + type2asm(child);
		sec_text.push_back(ln);
		
		assign_ax(dest_var, v);
		
	//Function calls
	} else if (child->type == AstType::FuncCall) {
		AstFuncCall *fc = dynamic_cast<AstFuncCall *>(child);
		build_func_call(fc);
		assign_ax(dest_var, v);
		
	//Array access
	} else if (child->type == AstType::ArrayAccess) {
		build_arr_access(child);
		assign_ax(dest_var, v);
		
	//Struct access
	} else if (child->type == AstType::StructAcc) {
		build_struct_acc(child);
		assign_ax(dest_var, v);
		
	//Integer math
	} else if (child->type == AstType::Math) {
		if (v.type == DataType::Int)
			build_int_math(node);
		
	//Raw types
	} else {
		ln = "mov " + asm_type(v);
		ln += " " + dest_var + ", " + type2asm(child);
		
		sec_text.push_back(ln);
		sec_text.push_back("");
	}
}

//Builds a math string
void Asm_x86::build_int_math(AstNode *node) {
	AstAttrNode *n = static_cast<AstAttrNode *>(node);
	AstNode *math = n->children.at(0);
	Var v = vars[n->get_name()];
	std::string ln = "";
	
	//Construct the destination variable
	std::string dest_var = "[" + get_reg("bp") + "-";
	dest_var += std::to_string(v.stack_pos) + "]";
	
	if (node->type == AstType::ArrayAssign)
		dest_var = build_arr_assign(node, v);
	else if (node->type == AstType::StructMod)
		dest_var = build_struct_mod(node);
	
	//Build the first element
	auto first = math->children.at(0);
	
	switch (first->type) {
		//ID
		case AstType::Id: {
			ln = "mov eax, " + type2asm(first);
		} break;
		
		//Int
		case AstType::Int: {
			AstInt *i = static_cast<AstInt *>(first);
			auto val = std::to_string(i->get_val());
			
			ln = "mov eax, " + val;
		} break;
		
		//Function calls
		case AstType::FuncCall: {
			AstFuncCall *fc = dynamic_cast<AstFuncCall *>(first);
			build_func_call(fc);
		} break;
		
		//Array access
		case AstType::ArrayAccess: {
			build_arr_access(first);
		} break;
		
		//Structure access
		case AstType::StructAcc: {
			build_struct_acc(first);
		} break;
		
		//Math
		case AstType::Math: {
			AstNode *n2 = node;
			n2->children.clear();
			n2->children.push_back(first);
			build_int_math(n2);
		} break;
	}
	
	sec_text.push_back(ln);
	
	//Now iterate through the reset of the children
	for (int i = 1; i<math->children.size(); i+=2) {
		auto op = math->children[i];
		auto next = math->children[i+1];
		std::string ln = "";
		
		bool is_mod = false;
		bool is_div = false;
		
		switch (op->type) {
			case AstType::Add: ln += "add eax, "; break;
			case AstType::Sub: ln += "sub eax, "; break;
			case AstType::Mul: ln += "imul eax, "; break;
			case AstType::Mod: is_mod = true;
			case AstType::Div: is_div = true; break;
		}
		
		if (is_div) {
			ln = "mov ebx, ";
		}
		
		if (next->type == AstType::Math) {
			if (x64)
				sec_text.push_back("push rax");
			else
				sec_text.push_back("push eax");
			
			AstNode *n2 = node;
			n2->children.clear();
			n2->children.push_back(next);
			build_int_math(n2);
			
			sec_text.push_back("mov ebx, eax");
			if (x64)
				sec_text.push_back("pop rax");
			else
				sec_text.push_back("pop eax");
			ln += "ebx";
		} else if (next->type == AstType::FuncCall) {
			sec_text.push_back("mov " + dest_var + ", eax");
			AstFuncCall *fc = dynamic_cast<AstFuncCall *>(next);
			build_func_call(fc);
			ln += dest_var;
		} else if (next->type == AstType::ArrayAccess) {
			sec_text.push_back("mov ebx, eax");
		
			build_arr_access(next);
			ln += "ebx";
		} else if (next->type == AstType::StructAcc) {
			sec_text.push_back("mov ebx, eax");
			
			build_struct_mod(next);
			ln += "ebx";
		} else {
			ln += type2asm(next);
		}
		
		sec_text.push_back(ln);
		
		if (is_div) {
			sec_text.push_back("cdq");
			sec_text.push_back("div ebx");
		}
		
		if (is_mod) {
			sec_text.push_back("mov eax, edx");
		}
	}
	
	//Finally, move it back
	ln = "mov " + asm_type(v);	
	ln += dest_var + ", eax";
	
	sec_text.push_back(ln);
	sec_text.push_back("");
}

//Builds a floating-point variable assignment
void Asm_x86::build_flt_assign(AstNode *node) {
	AstAttrNode *va = static_cast<AstAttrNode *>(node);
	AstNode *math = va->children.at(0);
	Var v = vars[va->get_name()];
	
	auto first = va->children.at(0);
	if (math->type == AstType::Math)
		first = math->children.at(0);
	
	switch (first->type) {
		//Assign a float value
		case AstType::Float: {
			auto name = build_float(first);
			sec_text.push_back("movss xmm0, [" + name + "]");
		} break;
		
		//Assign another variable
		case AstType::Id: {
			AstID *id = static_cast<AstID *>(first);
			Var v2 = vars[id->get_name()];
			
			std::string src = "movss xmm0, [" + get_reg("bp");
			src += "-" + std::to_string(v2.stack_pos) + "]";
			sec_text.push_back(src);
		} break;
		
		//Function call
		case AstType::FuncCall: {
			AstFuncCall *fc = static_cast<AstFuncCall *>(first);
			build_func_call(fc);
		} break;
		
		//Assign an array access
		case AstType::ArrayAccess: {
			build_arr_access(first);
		} break;
	}
	
	//If there are further children, we have math
	for (int i = 1; i<math->children.size(); i+=2) {
		auto op = math->children[i];
		auto next = math->children[i+1];
		std::string ln2 = "";
		
		switch (op->type) {
			case AstType::Add: ln2 = "addss xmm0, "; break;
			case AstType::Sub: ln2 = "subss xmm0, "; break;
			case AstType::Mul: ln2 = "mulss xmm0, "; break;
			case AstType::Div: ln2 = "divss xmm0, "; break;
		}
		
		switch (next->type) {
			case AstType::Float: {
				auto name = build_float(next);
				ln2 += "[" + name + "]";
			} break;
			
			case AstType::Id: {
				AstID *id = static_cast<AstID *>(next);
				Var v2 = vars[id->get_name()];
				
				ln2 += "[" + get_reg("bp") + "-";
				ln2 += std::to_string(v2.stack_pos) + "]";
			} break;
		}
		
		sec_text.push_back(ln2);
	}
	
	//Push the final result to the stack
	std::string ln = "movss [" + get_reg("bp") + "-";
	ln += std::to_string(v.stack_pos) + "], xmm0";
			
	sec_text.push_back(ln);
	sec_text.push_back("");
}

//Build SSE/AVX variables
void Asm_x86::build_floatex_assign(AstNode *node) {
	//Make sure we are not in 32-bit mode
	if (!x64) {
		std::cout << "Fatal: Vectorization is not supported in 32-bit mode." << std::endl;
		std::exit(1);
	}

	AstVarDec *vd = static_cast<AstVarDec *>(node);
	AstNode *math = vd->children.at(0);
	Var v = vars[vd->get_name()];
	int index = v.stack_pos;
	int size = 4;
		
	if (math->children.size() == 3) {
		auto part1 = static_cast<AstAttrNode *>(math->children.at(0));
		auto op = math->children.at(1);
		auto part2 = static_cast<AstAttrNode *>(math->children.at(2));
		
		Var v1 = vars[part1->get_name()];
		Var v2 = vars[part2->get_name()];
		
		DataType vd_type = vd->get_type();
		DataType op_type = v1.type;
		
		//Load variables
		std::string ln1 = "movups xmm0";
		std::string ln2 = "movups xmm1";
		
		std::string dest_var = " [rbp-" + std::to_string(index) + "], ";
		std::string dest_ln = "movups" + dest_var + "xmm0";
		
		if (op_type == DataType::Int256
			|| op_type == DataType::Float256
			|| vd_type == DataType::Int256) {
			ln1 = "vmovupd ymm0";
			ln2 = "vmovupd ymm1";
			dest_ln = "vmovups" + dest_var + "ymm0";
		}
		
		//Check if the first variable is an array
		if (v1.is_array) {
			sec_text.push_back("mov rax, [rbp-" + std::to_string(v1.stack_pos) + "]");
			ln1 += ", [rax]";
		} else {
			ln1 += ", [rbp-" + std::to_string(v1.stack_pos) + "]";
		}
		
		//Check if the second variable is an array
		if (v2.is_array) {
			sec_text.push_back("mov rbx, [rbp-" + std::to_string(v2.stack_pos) + "]");
			ln2 += ", [rbx]";
		} else {
			ln2 += ", [rbp-" + std::to_string(v2.stack_pos) + "]";
		}
		
		sec_text.push_back(ln1);
		sec_text.push_back(ln2);
		
		//Do the math
		//Scalar addition
		if (op->type == AstType::Add) {
			if (op_type == DataType::Float256)
				sec_text.push_back("vhaddps ymm0, ymm0, ymm1");
			else if (op_type == DataType::Float128)
				sec_text.push_back("haddps xmm0, xmm1");
			else if (op_type == DataType::Int256)
				sec_text.push_back("vphaddd ymm0, ymm0, ymm1");
			else
				sec_text.push_back("phaddd xmm0, xmm1");
			
		//Parallel addition
		} else if (op->type == AstType::Inc) {
			if (vd->get_type() == DataType::Int256
				|| vd->get_type() == DataType::Float256)
				sec_text.push_back("vaddps ymm0, ymm0, ymm1");
			else
				sec_text.push_back("addps xmm0, xmm1");
				
		//Parallel multiplication
		} else if (op->type == AstType::DMul) {
			if (vd->get_type() == DataType::Float256)
				sec_text.push_back("vmulps ymm0, ymm0, ymm1");
			else if (vd->get_type() == DataType::Float128)
				sec_text.push_back("mulps xmm0, xmm1");
			else if (vd->get_type() == DataType::Int256)
				sec_text.push_back("vpmulld ymm0, ymm0, ymm1");
			else
				sec_text.push_back("pmulld xmm0, xmm1");
		}
		
		sec_text.push_back("");
		
		//Store the result
		//If we have a 64-bit type, we have to add extraction code
		if (vd_type == DataType::Int64 || vd_type == DataType::Float64) {
			std::string d1 = "[rbp-" + std::to_string(index) + "]";
			std::string d2 = "[rbp-" + std::to_string(index-4) + "]";
			
			if (vd_type == DataType::Float64) {
				sec_text.push_back("extractps " + d1 + ", xmm0, 0");
				sec_text.push_back("extractps " + d2 + ", xmm0, 1");
			} else {
				sec_text.push_back("pextrd " + d1 + ", xmm0, 0");
				sec_text.push_back("pextrd " + d2 + ", xmm0, 1");
			}

		} else if (vd_type == DataType::Int128 && op_type == DataType::Int256) {
			sec_text.push_back("vextracti128 xmm4, ymm0, 0");
			sec_text.push_back("vextracti128 xmm5, ymm0, 1");
			sec_text.push_back("");
			
			std::string reg = "xmm4";
			bool l = 0;
			
			for (int i = 0; i<4; i++) {
				if (i == 2)
					reg = "xmm5";
					
				sec_text.push_back("pextrd eax, " + reg + ", " + std::to_string(l));
				sec_text.push_back("pinsrd xmm3, eax, " + std::to_string(i));
				sec_text.push_back("");
				
				l = !l;
			}
			
			std::string dest = "[rbp-" + std::to_string(index) + "]";
			sec_text.push_back("movups " + dest + ", xmm3");
			
		} else if (vd_type == DataType::Float128 && op_type == DataType::Float256) {
			std::string reg = "xmm4";
			bool l = 0;
			
			sec_text.push_back("vextractf128 xmm4, ymm0, 0");
			sec_text.push_back("vextractf128 xmm5, ymm0, 1");
			sec_text.push_back("");
		
			for (int i = 0; i<16; i+=4) {
				if (i == 8)
					reg = "xmm5";
					
				std::string ln = "extractps [rbp-" + std::to_string(index-i) + "], ";
				ln += reg + ", " + std::to_string(l);
				sec_text.push_back(ln);
					
				l = !l;
			}
		
			sec_text.push_back("");
		} else {
			sec_text.push_back(dest_ln);
		}
	} else {
		for (auto child : vd->children) {
			std::string dest = "[" + get_reg("bp") + "-";
			dest += std::to_string(index) + "]";
			
			std::string ln = "";
			
			//TODO: Add remaining types
			switch (child->type) {
				case AstType::Int: {
					AstInt *i = static_cast<AstInt *>(child);
					
					ln = "mov dword " + dest + ", " + std::to_string(i->get_val());
				} break;
				
				case AstType::Float: {
					auto flt_name = build_float(child);
					sec_text.push_back("movss xmm0, [" + flt_name + "]");
					
					ln = "movss " + dest + ", xmm0";
				} break;
			}
			
			index -= size;
			sec_text.push_back(ln);
		}
	}
	
	sec_text.push_back("");
}


