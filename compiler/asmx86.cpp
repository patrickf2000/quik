#include <iostream>
#include <fstream>
#include <types.hh>

#include "asmx86.hh"

//Set things up
Asm_x86::Asm_x86(std::string p) {
	path = p;
	
	extern_data.push_back("global _start");
}

//Iterate through the tree and assemble
void Asm_x86::assemble(AstNode *top) {
	for (auto node : top->children) {
		if (node->type == AstType::FuncDec) {
			build_function(node);
			assemble(node);
		} else if (node->type == AstType::VarDec) {
			build_var_dec(node);
		}
	}
}

//Assembles a function
void Asm_x86::build_function(AstNode *node) {
	AstFuncDec *fd = dynamic_cast<AstFuncDec *>(node);
	
	std::string ln = fd->get_name();
	if (ln == "main") {
		ln = "_start";
	}
	
	ln += ":";
	sec_text.push_back(ln);
}

//Assembles a variable declaration
void Asm_x86::build_var_dec(AstNode *node) {
	AstVarDec *vd = dynamic_cast<AstVarDec *>(node);
	std::string ln = vd->get_name() + " ";
	
	switch (vd->get_type()) {
		case DataType::Byte:
		case DataType::Char:
		case DataType::Bool:
		case DataType::Str: ln += "db "; break;
		case DataType::Short: ln += "dw "; break;
		case DataType::Int: ln += "dd "; break;
		case DataType::Long:
		case DataType::Float: ln += "dq "; break;
	}
	
	auto first = vd->children.at(0);
	
	if (first->type == AstType::Id) {
	
	} else {
		if (first->type == AstType::Int) {
			AstInt *i = dynamic_cast<AstInt *>(first);
			ln += std::to_string(i->get_val());
		}
	}
	
	sec_data.push_back(ln);
}

//Write out the final product
void Asm_x86::write() {
	std::ofstream writer(path);
	
	writer << "section .data" << std::endl;
	
	for (auto l : sec_data) {
		writer << "\t" << l << std::endl;
	}
	
	writer << std::endl;
	writer << "section .text" << std::endl;
	
	for (auto l : extern_data) {
		writer << "\t" << l << std::endl;
	}
	
	writer << std::endl;
	
	for (auto l : sec_text) {
		if (l[l.length()-1] != ':') {
			writer << "\t";
		}
		writer << l << std::endl;
	}
}
