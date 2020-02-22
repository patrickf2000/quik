#include <iostream>
#include <fstream>
#include <cstdlib>

#include <ast.hh>
#include <build.hh>
#include <ltac/ltac.hh>
#include <ltac/ltac_build.hh>

#include "compiler.hh"
#include "utils.hh"

#include "x64/asm_x64.hh"
#include "arm7/asm_arm7.hh"

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
			case CpuArch::Intel64: {
				Asm_x64 asm_builder(file);
				
				if (config.out_type == BuildType::DynLib)
					asm_builder.build_PIC();
				
				asm_builder.write();
			} break;
			
			case CpuArch::Intel32: break;
			case CpuArch::Arm7: {
				Asm_Arm7 asm_builder(file);
				
				if (config.out_type == BuildType::DynLib)
					asm_builder.build_PIC();
					
				asm_builder.write();
			} break;
		}
		
		delete top;
		delete builder;
		delete file;
	}
}

//Assemble the assembly
//TODO: Update not to use system calls
void Compiler::compile() {
	for (int i = 0; i<asm_files.size(); i++) {
		std::string cmd = "as " + asm_files[i] + " -o ";
		cmd += obj_files[i];
		
		system(cmd.c_str());
	}
}

//Link the object files
//TODO: Do not use the system() function
void Compiler::link() {
	std::string ld_line = "ld ";

	//Link an executable
	if (config.out_type == BuildType::Exe) {
		ld_line += "/usr/lib/x86_64-linux-gnu/crti.o ";
		ld_line += "/usr/lib/x86_64-linux-gnu/crtn.o ";
		ld_line += "/usr/lib/x86_64-linux-gnu/crt1.o -lc ";
		ld_line += "-lqkstdlib ";

		for (auto obj : obj_files) {
			ld_line += obj + " ";
		}
		
		ld_line += "-dynamic-linker /lib64/ld-linux-x86-64.so.2 ";
		ld_line += "-o " + config.output;
		
	//Link a dynamic library
	} else if (config.out_type == BuildType::DynLib) {
		for (auto obj : obj_files) {
			ld_line += obj + " ";
		}
		
		ld_line += "-o lib" + config.output + ".so ";
		ld_line += "-shared -lc";
	}
	
	system(ld_line.c_str());
}


