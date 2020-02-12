#include <fstream>
#include <string>
#include <iostream>

#include "ltac_file.hh"
#include "ltac.hh"

void write(LtacFile *file) {
	std::ofstream writer(file->path);
	
	writer << ".intel_syntax noprefix" << std::endl;
	writer << ".data" << std::endl;
	writer << std::endl;
	writer << ".text" << std::endl;
	writer << "\t.global main" << std::endl;
	writer << std::endl;
	
	for (auto node : file->children) {
		switch (node->type) {
			case LTAC::Label: {
				writer << "lbl " << node->val << std::endl;
			} break;
			
			case LTAC::Setup: {
				writer << "\tsetup" << std::endl;
				writer << std::endl;
			} break;
		}
	}
	
	writer.close();
}
