#include <iostream>
#include <string>

#include "ltac.hh"

std::string code2str(LtacNode *code_ln, bool child=false) {
	std::string content = "";

	switch (code_ln->type) {
		case ltac::Func: {
			auto fd = static_cast<LtacFunc *>(code_ln);
			content += "func " + fd->name + "\n";
		} break;
		
		case ltac::Ret: {
			content += "\tret\n\n";
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
		
		case ltac::Int: {
			auto li = static_cast<LtacInt *>(code_ln);
			content += std::to_string(li->val);
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
