#include <iostream>

#include "asm_x64.hh"

//The constructor where we set everything up
Asm_x64::Asm_x64(LtacFile *f) {
	file = f;
	writer = std::ofstream(file->name);
}

//Write out the assembly code
void Asm_x64::write() {
	writer << ".intel_syntax noprefix" << std::endl;
	writer << ".data" << std::endl;
	
	build_data(file->data);
	
	writer << std::endl;
	writer << ".text" << std::endl;
	writer << ".global main" << std::endl;
	writer << std::endl;
	
	build_code(file->code);
	
	writer << std::endl;
	writer.close();
}

//Build the data section
void Asm_x64::build_data(LtacDataSec *data) {
	for (auto ln : data->children) {
		switch (ln->type) {
			case ltac::String: {
				auto lstr = static_cast<LtacString *>(ln);
				writer << "\t" << lstr->name << ": .string \"";
				writer << lstr->val << "\"\n";
			} break;
		}
	}
}

//Build the code section
void Asm_x64::build_code(LtacCodeSec *code) {
	for (auto ln : code->children) {
		switch (ln->type) {
			case ltac::Func: build_func(ln); break;
			
			case ltac::Ret: {
				writer << std::endl;
				writer << "\tleave" << std::endl;
				writer << "\tret" << std::endl;
				writer << std::endl;
			} break;
			
			case ltac::Var: build_var(ln); break;
		}
	}
}

//Build a function declaration
void Asm_x64::build_func(LtacNode *node) {
	auto fd = static_cast<LtacFunc *>(node);
	writer << fd->name << ":" << std::endl;
	
	//Setup the stack
	writer << "\tpush rbp" << std::endl;
	writer << "\tmov rbp, rsp" << std::endl;
	writer << std::endl;
}

//Build a variable declaration
void Asm_x64::build_var(LtacNode *node) {
	auto var = static_cast<LtacVar *>(node);
	auto src = node->children[0];
	
	switch (src->type) {
		//Integers
		case ltac::Int: {
			writer << "\tmov DWORD PTR [rbp-";
			writer << var->pos << "], ";
			
			auto li = static_cast<LtacInt *>(src);
			writer << li->val << std::endl;
		} break;
		
		//Strings
		case ltac::String: {
			writer << "\tmov QWORD PTR [rbp-";
			writer << var->pos << "], ";
			writer << "OFFSET FLAT:";
			
			auto lstr = static_cast<LtacString *>(src);
			writer << lstr->name << std::endl;
		} break;
		
		//Variables
		case ltac::Var: {
			auto var2 = static_cast<LtacVar *>(src);
			writer << std::endl;
			writer << "\tmov eax, [rbp-" << var2->pos;
			writer << "]" << std::endl;
			
			writer << "\tmov DWORD PTR [rbp-";
			writer << var->pos << "], eax" << std::endl;
		} break;
	}
}

