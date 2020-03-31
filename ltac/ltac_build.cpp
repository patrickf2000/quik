#include "ltac_build.hh"

//Creates an LTAC file
LtacFile *LTAC_Builder::build_file(AstNode *top) {
	file = new LtacFile("out");
	
	auto data = new LtacDataSec;
	auto code = new LtacCodeSec;
	
	file->data = data;
	file->code = code;
	
	assemble(top);
	
	return file;
}

//The main assembly function for ltac
void LTAC_Builder::assemble(AstNode *top) {
	for (auto node : top->children) {
		switch (node->type) {
			case AstType::Scope: assemble(node); break;
			
			case AstType::ExternFunc: build_func(node, true); break;
			
			//Function declarations
			case AstType::FuncDec: {
				if (!has_func_calls(node))
					enable_rvar = true;
			
				auto func = build_func(node);
				assemble(node);
				
				enable_rvar = false;
				rvar_index = 0;
				
				int stack_size = 0;
				if (stack_pos > 0) {
					while (stack_size < (stack_pos + 1))
						stack_size += 16;
				}
				
				func->stack_size = stack_size;
				stack_pos = 0;
			} break;
			
			case AstType::Return: build_ret(node); break;
			
			case AstType::FuncCall: {
				auto l_fc = build_func_call(node);
				file->code->children.push_back(l_fc);
			} break;
			
			//Variables
			case AstType::VarDec: build_var_dec(node);
			case AstType::VarAssign: build_var_assign(node); break;
			case AstType::MultiVarAssign: {
				build_multi_var_assign(node);
			} break;
			
			//Arrays
			case AstType::ArrayDec: build_array_dec(node); break;
			
			//Comparisons
			case AstType::If: 
			case AstType::Elif: 
			case AstType::Else: {
				std::string name = "L" + std::to_string(lbl_count);
				++lbl_count;
				labels.push(name);
				
				if (node->type == AstType::If) {
					std::string end_name = "L" + std::to_string(lbl_count);
					++lbl_count;
					end_lbls.push(end_name);	
				}
				
				if (node->type != AstType::Else)
					build_cmp(node);
				else
					assemble(node);
				
				auto lbl = new LtacLabel(name);
				file->code->children.push_back(lbl);
			} break;
			
			case AstType::EndIf: {
				auto name = end_lbls.top();
				end_lbls.pop();
				
				auto lbl = new LtacLabel(name);
				file->code->children.push_back(lbl);
			} break;
			
			//Loops
			case AstType::While: build_while(node); break;
			case AstType::Loop: build_loop(node); break;
		}
	}
}

//Builds a string variable (utility function)
LtacNode *LTAC_Builder::build_string(AstNode *node) {
	auto str = static_cast<AstString *>(node);
	auto lstr = new LtacString;
	std::string val = str->get_val();

	if (dec_strings.find(val) != dec_strings.end()) {
		lstr->name = dec_strings[val];
		lstr->val = val;
		return lstr;
	}

	std::string name = "STR_" + std::to_string(str_count);
	++str_count;

	lstr->name = name;
	lstr->val = str->get_val();
	
	dec_strings[lstr->val] = lstr->name;

	file->data->children.push_back(lstr);
	return lstr;
}

//Builds a float variable (utility function)
LtacNode *LTAC_Builder::build_float(AstNode *node) {
	auto flt = static_cast<AstFloat *>(node);
	auto l_flt = new LtacFloat;
	float val = flt->get_val();
	
	std::string name = "FLT_" + std::to_string(flt_count);
	++flt_count;
	
	l_flt->name = name;
	l_flt->val = val;
	
	char buf[32];
	sprintf(buf, "%d", *(unsigned int*)&val);
	l_flt->i_val = std::stoi(std::string(buf));
	
	dec_flt[val] = l_flt->name;
	
	file->data->children.push_back(l_flt);
	return l_flt;
}

//Increments the stack based on a datatype
void LTAC_Builder::inc_stack(DataType type) {
	switch (type) {
		case DataType::Byte:
		case DataType::Char: stack_pos += 1; break;
		case DataType::Bool:
		case DataType::Int: 
		case DataType::Float: stack_pos += 4; break;
		case DataType::Str: stack_pos += ptr_size; break;
		case DataType::Int128:
		case DataType::Float128: stack_pos += 16; break;
		case DataType::Int256:
		case DataType::Float256: stack_pos += 32; break;
	}
}


