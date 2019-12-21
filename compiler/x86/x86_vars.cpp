//x86_vars
//Contains the functions for assembling variable-specific stuff
#include "asmx86.hh"

//Assembles a variable declaration
void Asm_x86::build_var_dec(AstNode *node) {
	AstVarDec *vd = dynamic_cast<AstVarDec *>(node);
	std::string ln = vd->get_name() + " ";
	
	switch (vd->get_type()) {
		case DataType::Byte:
		case DataType::Char:
		case DataType::Str: ln += "db "; break;
		case DataType::Short: ln += "dw "; break;
		case DataType::Bool:
		case DataType::Int: ln += "dd "; break;
		case DataType::Long:
		case DataType::Float: ln += "dq "; break;
	}
	
	auto first = vd->children.at(0);
	
	if (first->type == AstType::Id || first->type == AstType::FuncCall 
			|| first->type == AstType::ArrayAccess
			|| vd->children.size() > 1) {
		ln += "0";
		build_var_assign(node);
	} else {
		if (first->type == AstType::Int) {
			AstInt *i = dynamic_cast<AstInt *>(first);
			ln += std::to_string(i->get_val());
		} else if (first->type == AstType::Str) {
			AstString *i = dynamic_cast<AstString *>(first);
			ln += "\"" + i->get_val() + "\",0";
		} else if (first->type == AstType::Float) {
			AstFloat *i = dynamic_cast<AstFloat *>(first);
			ln += std::to_string(i->get_val());
		} else if (first->type == AstType::Bool) {
			AstBool *i = dynamic_cast<AstBool *>(first);
			if (i->get_val())
				ln += "1";
			else
				ln += "0";
		}
	}
	
	sec_data.push_back(ln);
}

//Builds a variable assignment
void Asm_x86::build_var_assign(AstNode *node) {
	std::string dest_var = "";
	std::vector<AstNode *> children;

	//Variable assignments
	if (node->type == AstType::VarDec || node->type == AstType::VarAssign) {
		AstVarDec *va = dynamic_cast<AstVarDec *>(node);
		if (va->get_type() == DataType::Float) {
			build_flt_assign(node);
			return;
		}
		
		dest_var = "[" + va->get_name() +"]";
		children = va->children;
		
	//Array assignments
	} else if (node->type == AstType::ArrayAssign) {
		AstArrayAssign *assign = dynamic_cast<AstArrayAssign *>(node);
		dest_var = "[" + assign->get_name() + "+";
		
		//Math for int: 4 * the index
		switch (assign->index->type) {
			case AstType::Id: {
				AstID *i = dynamic_cast<AstID *>(assign->index);
				dest_var += "ebx]";
				
				sec_text.push_back("mov ebx, [" + i->get_name() + "]");
				sec_text.push_back("imul ebx, 4");
			} break;
			
			case AstType::Int: {
				AstInt *i = dynamic_cast<AstInt *>(assign->index);
				int val = i->get_val();
				val *= 4;
				dest_var += std::to_string(val) + "]";
			} break;
		}
		
		children = node->children;
	}
	
	//Insert the first element
	auto child = node->children.at(0);
			
	switch (child->type) {
		//Integers
		case AstType::Int: {
			AstInt *i = dynamic_cast<AstInt *>(child);
			auto val = std::to_string(i->get_val());
		
			sec_text.push_back("mov eax, " + val);
		} break;
		
		//Other variables
		case AstType::Id: {
			AstID *id = dynamic_cast<AstID *>(child);
			sec_text.push_back("mov eax, [" + id->get_name() + "]");
		} break;
		
		//Function calls
		case AstType::FuncCall: {
			AstFuncCall *fc = dynamic_cast<AstFuncCall *>(child);
			build_func_call(fc);
		} break;
		
		//Array access
		case AstType::ArrayAccess: {
			build_arr_access(child);
		} break;
	}
	
	//Iterate through all the children
	for (int i = 1; i<children.size(); i+=2) {
		auto current = children.at(i);
		auto next = children.at(i+1);
		std::string ln = "";
		
		bool is_mod = false;
		bool is_div = false;
		
		switch (current->type) {
			case AstType::Add: ln += "add eax, "; break;
			case AstType::Sub: ln += "sub eax, "; break;
			case AstType::Mul: ln += "imul eax, "; break;
			case AstType::Mod: is_mod = true;
			case AstType::Div: is_div = true; break;
		}
		
		if (is_div) {
			ln = "mov ebx, ";
		}
		
		if (next->type == AstType::FuncCall) {
			sec_text.push_back("mov " + dest_var + ", eax");
			AstFuncCall *fc = dynamic_cast<AstFuncCall *>(next);
			build_func_call(fc);
			ln += dest_var;
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
	
	sec_text.push_back("mov " + dest_var + ", eax");
	sec_text.push_back("");
}

//Builds a floating-point variable assignment
void Asm_x86::build_flt_assign(AstNode *node) {
	AstAttrNode *va = dynamic_cast<AstAttrNode *>(node);
	auto child = va->children.at(0);
	
	type2flt(child);
	
	//Iterate through the children
	for (int i = 1; i<va->children.size(); i+=2) {
		auto current = va->children.at(i);
		auto next = va->children.at(i+1);
		
		type2flt(next);
		
		switch (current->type) {
			case AstType::Add: sec_text.push_back("fadd st0, st1"); break;
			case AstType::Sub: sec_text.push_back("fsubp st1, st0"); break;
			case AstType::Mul: sec_text.push_back("fmulp st1, st0"); break;
			case AstType::Div: sec_text.push_back("fdivp st1, st0"); break;
		}
	}
	
	sec_text.push_back("fstp qword [" + va->get_name() + "]");
	sec_text.push_back("");
}

