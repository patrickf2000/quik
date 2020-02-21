#include "asm_x64.hh"

//Registers for function calls
std::string call_regs[] = {
	"rdi",
	"rsi",
	"rdx",
	"rcx",
	"r8",
	"r9"
};

std::string call_regs32[] = {
	"edi",
	"esi",
	"edx",
	"ecx",
	"r8",
	"r9"
};

std::string call_flt_regs[] = {
	"xmm0",
	"xmm1",
	"xmm2",
	"xmm3",
	"xmm4",
	"xmm5",
	"xmm6",
	"xmm7"
};

//Build a function declaration
void Asm_x64::build_func(LtacNode *node) {
	auto fd = static_cast<LtacFunc *>(node);
	
	if (fd->is_global) {
		writer << ".global " << fd->name << std::endl;
		writer << ".type " << fd->name << ", @function";
		writer << std::endl << std::endl;
	} else if (fd->name == "main") {
		writer << ".global main" << std::endl;
		writer << std::endl;
	}
	
	writer << fd->name << ":" << std::endl;
	
	//Setup the stack
	writer << "\tpush rbp" << std::endl;
	writer << "\tmov rbp, rsp" << std::endl;
	writer << std::endl;
	
	//Retrieve the arguments
	int call_index = 0;
	int call_index_flt = 0;
	
	for (auto a : fd->children) {
		auto arg = static_cast<LtacVar *>(a);
		
		switch (arg->d_type) {
			//Integers
			case DataType::Int: {
				writer << "\tmov DWORD PTR ";
				if (pic) {
					writer << "-" << arg->pos << "[rbp], ";
				} else {
					writer <<"[rbp-" << arg->pos << "], ";
				}
				writer << call_regs32[call_index] << std::endl;
			} break;
			
			//Strings
			case DataType::Str: {
				writer << "\tmov QWORD PTR ";
				if (pic) {
					writer << "-" << arg->pos << "[rbp], ";
				} else {
					writer <<"[rbp-" << arg->pos << "], ";
				}
				writer << call_regs[call_index] << std::endl;
			} break;
		}
		
		++call_index;
	}
	
	if (fd->children.size() > 0)
		writer << std::endl;
}

//Build a function call
void Asm_x64::build_func_call(LtacNode *node) {
	auto fc = static_cast<LtacFunc *>(node);
	
	//Add the arguments
	int call_index = 0;
	int call_index_flt = 0;
	
	for (auto arg : fc->children) {
		switch (arg->type) {
			//Raw string arguments
			case ltac::String: {
				auto lstr = static_cast<LtacString *>(arg);
				auto reg = call_regs[call_index];
				++call_index;
				
				if (pic) {
					writer << "\tlea " << reg << ", ";
					writer << lstr->name << "[rip]" << std::endl;
				} else {
					writer << "\tmov " << reg << ", ";
					writer << "OFFSET FLAT:" << lstr->name << std::endl;
				}
			} break;
			
			//TODO: Add the rest
		}
	}
	
	//Call the function
	writer << "\tcall " << fc->name;
	if (pic)
		writer << "@PLT";
	writer << std::endl << std::endl;
}
