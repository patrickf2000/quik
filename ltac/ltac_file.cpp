#include <fstream>
#include <string>
#include <iostream>

#include "ltac_file.hh"
#include "ltac.hh"

std::string translate(LtacNode *part) {
	std::string ln = "";

	switch (part->type) {
		//Memory
		case LTAC::Mem: {
			LtacMem *mem = static_cast<LtacMem *>(part);
			ln = "[base+" + std::to_string(mem->index) + "*"
				+ std::to_string(mem->scale) + "]";
		} break;
		
		//Pointer
		case LTAC::Ptr: {
			ln = "*" + part->val;
		} break;
		
		//Integers
		case LTAC::Int: {
			LtacInt *i = static_cast<LtacInt *>(part);
			ln = std::to_string(i->get_val());
		} break;
		
		//Argument register
		case LTAC::ArgReg: {
			ln = "arx";
		} break;
		
		//TODO: Add the rest
	}
	
	return ln;
}

void write(LtacFile *file) {
	std::ofstream writer(file->path);
	
	writer << ".data" << std::endl;
	writer << std::endl;
	writer << ".code" << std::endl;
	
	for (auto node : file->children) {
		switch (node->type) {
			case LTAC::None: {
				writer << std::endl;
			} break;
		
			case LTAC::Extern: {
				writer << "extern " << node->val << std::endl;
			} break;
		
			case LTAC::Label: {
				writer << std::endl;
				writer << "lbl " << node->val << std::endl;
			} break;
			
			case LTAC::Call: {
				writer << "\tcall " << node->val << std::endl;
				writer << std::endl;
			} break;
			
			case LTAC::Setup: {
				writer << "\tsetup" << std::endl;
				writer << std::endl;
			} break;
			
			case LTAC::Mov: {
				writer << "\tmov " << translate(node->args[0])
					<< ", " << translate(node->args[1]) << std::endl;
			} break;
			
			case LTAC::Leave: {
				writer << "\tleave" << std::endl;
			} break;
			
			case LTAC::Ret: {
				writer << "\tret" << std::endl;
				writer << std::endl;
			} break;
		}
	}
	
	writer.close();
}
