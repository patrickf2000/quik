#include "asm_arm7.hh"

//Build the data section
void Asm_Arm7::build_data(LtacDataSec *data) {
	for (auto node : data->children) {
		switch (node->type) {
			case ltac::String: {
				auto str = static_cast<LtacString *>(node);
				std::string d_name = ".d_" + str->name;
				
				writer << d_name << ": .ascii \"" << str->val;
				writer << "\\000\"" << std::endl;
				
				std::string ln = str->name + ": .word " + d_name;
				str_labels.push_back(ln);
			} break;
			
			//TODO: Add the rest
		}
	}
}

//Build the code section
void Asm_Arm7::build_code(LtacCodeSec *code) {
	for (auto ln : code->children) {
		switch (ln->type) {
			case ltac::Func: build_func(ln); break;
			case ltac::FuncCall: build_func_call(ln); break;
			case ltac::Ret: build_ret(ln); break;
		}
	}
	
	//Write out the data labels
	for (auto ln : str_labels)
		writer << ln << std::endl;
	writer << std::endl;
}


