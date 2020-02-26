#include <iostream>
#include <string>

#include "ltac.hh"

std::string code2str(LtacNode *code_ln, bool child=false) {
	std::string content = "";

	switch (code_ln->type) {
		case ltac::Label: {
			auto lbl = static_cast<LtacLabel *>(code_ln);
			content += "lbl " + lbl->name + "\n";
		} break;
	
		case ltac::Func: {
			auto fd = static_cast<LtacFunc *>(code_ln);
			if (fd->is_extern)
				content += "extern ";
			content += "func " + fd->name + "\n";
			
			for (auto arg : fd->children) {
				content += "\tldarg " + code2str(arg, true);
				content += "\n";
			}
			
			if (fd->children.size() > 0)
				content += "\n";
		} break;
		
		case ltac::Ret: {
			content += "\tret ";
			
			if (code_ln->children.size() > 0) {
				content += "(";
				for (auto arg : code_ln->children) {
					content += code2str(arg, true);
					content += ", ";
				}
				content += ")";
			}
			
			content += "\n\n";
		} break;
		
		case ltac::FuncCall: {
			auto fc = static_cast<LtacFuncCall *>(code_ln);
			
			if (fc->ret_dest.size() > 0 || child) {
				if (!child)
					content += "\t";
				for (auto var : fc->ret_dest) {
					content += "[bp-" + std::to_string(var->pos) + "] ";
				}
				content += "= ";
			} else {
				content += "\tcall ";
			}
			
			content += fc->name + " (";
			
			for (auto arg : fc->children) {
				content += code2str(arg, true);
				content += ",";
			}
			
			content += ")";
			
			if (!child)
				content += "\n\n";
		} break;
		
		case ltac::Var: {
			auto var = static_cast<LtacVar *>(code_ln);
			std::string v_str = "[bp+" + std::to_string(var->pos) + "]";
			
			if (child) {
				content = v_str;
			} else {
				content = "\tmov " + v_str + ", ";
				content += code2str(var->children[0], true);
				content += "\n";
			}
		} break;
		
		case ltac::Byte:
		case ltac::Int: {
			auto li = static_cast<LtacInt *>(code_ln);
			content += std::to_string(li->val);
		} break;
		
		case ltac::Float: {
			auto lf = static_cast<LtacFloat *>(code_ln);
			content += lf->name;
		} break;
		
		case ltac::String: {
			auto lstr = static_cast<LtacString *>(code_ln);
			content += lstr->name;
		} break;
		
		case ltac::Math: {
			auto math = static_cast<LtacMath *>(code_ln);
			content += code2str(math->init_val, true) + "\n";
			
			for (auto op : math->children) {
				content += code2str(op);
			}
		} break;
		
		case ltac::MathOp: {
			auto math_op = static_cast<LtacMathOp *>(code_ln);
			content += "\t";
			
			switch (math_op->op) {
				case Operator::Add: content += "add reg, "; break;
				case Operator::Sub: content += "sub reg, "; break;
				case Operator::Mul: content += "mul reg, "; break;
				case Operator::Div: content += "div reg, "; break;
				case Operator::Mod: content += "mod reg, "; break;
			}
			
			content += code2str(math_op->operand, true);
			content += "\n";
		} break;
		
		case ltac::Push: content += "\tpush reg\n"; break;
		case ltac::Pop: content += "\tpop reg\n"; break;
		
		case ltac::ICmp:
		case ltac::Cmp: {
			auto cmp = static_cast<LtacCmp *>(code_ln);
			
			if (code_ln->type == ltac::ICmp)
				content += "\ticmp ";
			else
				content += "\tcmp ";
			
			content += code2str(cmp->lval, true);
			content += ", ";
			content += code2str(cmp->rval, true);
			content += "\n";
		} break;
		
		case ltac::Jmp: {
			auto jmp = static_cast<LtacJmp *>(code_ln);
			
			switch (jmp->op) {
				case Operator::None: content += "\tjmp "; break;
				case Operator::Equal: content += "\tje "; break;
				case Operator::NotEqual: content += "\tjne "; break;
				case Operator::Greater: content += "\tjg "; break;
				case Operator::Less: content += "\tjl "; break;
				case Operator::GreaterEq: content += "\tjge "; break;
				case Operator::LessEq: content += "\tjle "; break;
			}
			
			content += jmp->dest + "\n";
		} break;
	}
	
	return content;
}

//Translates an LTAC IR to a string for printing or writing
std::string ltac2str(LtacFile *file) {
	std::string content = "";
	content += ".file: " + file->name + "\n";
	content += ".data\n";
	
	//Print the data
	for (auto code_ln : file->data->children) {
		switch (code_ln->type) {
			//Floats
			case ltac::Float: {
				auto lf = static_cast<LtacFloat *>(code_ln);
				content += "\t" + lf->name + ": word ";
				content += std::to_string(lf->val) + "\t#";
				content+= std::to_string(lf->i_val) + "\n";
			} break;
		
			//Strings
			case ltac::String: {
				auto lstr = static_cast<LtacString *>(code_ln);
				content += "\t";
				content += lstr->name + ": string \"" + lstr->val + "\"\n";
			} break;
		}
	}
	
	content += "\n.code\n";
	
	//Print the code
	for (auto code_ln : file->code->children) {
		content += code2str(code_ln);
	}
	
	return content;
}

//Prints an ltac file to the console
void print_ltac(LtacFile *file) {
	std::string content = ltac2str(file);
	std::cout << content << std::endl;
}
