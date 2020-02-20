#include <iostream>
#include <fstream>
#include <cstdlib>

#include <ast.hh>
#include <build.hh>
#include <ltac/ltac.hh>
#include <ltac/ltac_build.hh>

#include "compiler.hh"
#include "utils.hh"

Compiler::Compiler(Config c) {
	config = c;
}

//Setup all the inputs
void Compiler::set_inputs(std::vector<std::string> inputs) {
	src_files = inputs;

	for (auto in : inputs) {
		std::string path = get_path(in);
		std::string base = get_basename(in);
		
		std::string asm_path = "/tmp/" + base + ".asm";
		std::string obj_path = "/tmp/" + base + ".o";
		
		asm_files.push_back(asm_path);
		obj_files.push_back(obj_path);
	}
}

//Generates assembly
void Compiler::assemble() {
	for (int i = 0; i<src_files.size(); i++) {
		auto lines = load_source(src_files[i].c_str());
		AstNode *top = build_ast(lines, true, false);
		
		LTAC_Builder *builder = new LTAC_Builder;
		LtacFile *file = builder->build_file(top);
		file->name = asm_files[i];
		
		switch (config.arch) {
			case CpuArch::Intel64: break;
			case CpuArch::Intel32: break;
			case CpuArch::Arm7: break;
		}
		
		delete top;
		delete builder;
		delete file;
	}
}
