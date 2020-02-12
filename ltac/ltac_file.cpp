#include <fstream>
#include <string>
#include <iostream>

#include "ltac_file.hh"
#include "ltac.hh"

void write(LtacFile *file) {
	std::ofstream writer(file->path);
	
	writer << ".data" << std::endl;
	writer << std::endl;
	writer << ".code" << std::endl;
	
	for (auto node : file->children) {
		switch (node->type) {
			case LTAC::Extern: {
				writer << "extern " << node->val << std::endl;
			} break;
		
			case LTAC::Label: {
				writer << std::endl;
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
