#include <iostream>
#include <string>

#include "ltac.hh"

//Translates an LTAC IR to a string for printing or writing
std::string ltac2str(LtacFile *file) {
	std::string content = "";
	content += ".file: " + file->name + "\n";
	content += ".data\n";
	
	//Print the data
	
	content += "\n.code\n";
	
	//Print the code
	for (auto code_ln : file->code->children) {
		switch (code_ln->type) {
			case ltac::Func: {
				auto fd = static_cast<LtacFunc *>(code_ln);
				content += "func " + fd->name + "\n";
			} break;
			
			case ltac::Ret: {
				content += "\tret\n\n";
			} break;
		}
	}
	
	return content;
}

//Prints an ltac file to the console
void print_ltac(LtacFile *file) {
	std::string content = ltac2str(file);
	std::cout << content << std::endl;
}
