#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include <cc_config.hh>

#include "config.hh"
#include "compiler.hh"

void version();
void help();

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "Fatal: No input files" << std::endl;
		return 1;
	}
	
	//Holds our source files
	std::vector<std::string> inputs;
	
	//Holds configuration options
	Config config;
	
	config.output = "out";					// -o
	config.asm_only = false;				// -s
	config.obj_only = false;				// -c
	config.optimize = false;				// --optimize
	
	config.arch = CpuArch::Intel64;			// -m <x86_64, i386, armv7>
	config.asm_type = Asm::Gas;					// -a <gas, nasm>
	config.out_type = BuildType::Exe;		// (nothing for executable)
											// --static (for static library)
											// --shared (for shared library)
	
	for (int i = 1; i<argc; i++) {
		std::string arg = std::string(argv[i]);
		
		if (arg == "-o") {
			config.output = std::string(argv[i+1]);
			++i;
		} else if (arg == "-m") {
			//TODO
		} else if (arg == "--shared") {
			config.out_type = BuildType::DynLib;
		} else if (arg == "-h") {
			help();
		} else if (arg == "-v") {
			version();
		} else if (arg[0] == '-') {
			std::cout << "Error: Unknown command line option." << std::endl;
			std::cout << std::endl;
			help();
		} else {
			inputs.push_back(arg);
		}
	}
	
	//Compile
	Compiler *compiler = new Compiler(config);
	compiler->set_inputs(inputs);
	compiler->assemble();
	compiler->compile();
	compiler->link();
	
	delete compiler;
	
	return 0;
}

void version() {
	std::cout << "Quikc Version " << VERSION << std::endl;
	std::cout << std::endl;
	std::cout << "Quikc is the compiler for the Quik programming language." << std::endl;
	std::cout << "The compiler and the parser library were written by Patrick Flynn." << std::endl;
	std::cout << "The program is licensed under the BSD-3 license." << std::endl;
	std::cout << std::endl;
	std::cout << "See patrickflynn.co/quik for program information and documentation." << std::endl;
	std::cout << "The source can be found at github.com/patrickf2000/quik" << std::endl;
	std::cout << std::endl;
	std::exit(1);
}

void help() {
	std::cout << "Usage quikc [OPTIONS] input1, input2, ..." << std::endl;
	std::cout << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "\t-o\t\tFile output name." << std::endl;
	std::cout << "\t-s\t\tOutput Assembly only." << std::endl;
	std::cout << "\t-c\t\tOutput object code only." << std::endl;
	std::cout << "\t--optimize\tProduce optimized code (experimental)." << std::endl;
	std::cout << std::endl;
	std::cout << "\t-m <TYPE>\tSpecify CPU architecture." << std::endl;
	std::cout << "\t\tx86_64\tIntel x86 64-bit Assembly. (default)" << std::endl;
	std::cout << "\t\ti386\tIntel x86 32-bit Assembly." << std::endl;
	std::cout << "\t\tarmv7\tArm v7 Assembly." << std::endl;
	std::cout << std::endl;
	std::cout << "\t-a <TYPE>\tSpecify which Assembler to use." << std::endl;
	std::cout << "\t\tgas\tUse the GNU assembler (default)." << std::endl;
	std::cout << "\t\tnasm\tUse the NASM assembler." << std::endl;
	std::cout << std::endl;
	std::cout << "\t-t <TYPE>\tSpecify output build type." << std::endl;
	std::cout << "\t\texe\tBuild an executable program (default)." << std::endl;
	std::cout << "\t\tslib\tBuild a static library." << std::endl;
	std::cout << "\t\tdlib\tBuild a dynamic library."<< std::endl;
	std::cout << std::endl;
	std::cout << "\t-v\t\tDisplay version information and exit." << std::endl;
	std::cout << "\t-h\t\tDisplay this message and exit." << std::endl;
	std::cout << std::endl;
}

