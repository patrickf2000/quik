#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include <ast.hh>
#include <build.hh>

#include "utils.hh"
#include "asmx86.hh"

void help() {
	std::cout << "help" << std::endl;
	std::exit(1);
}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "Fatal: No input specified!" << std::endl;
		std::exit(1);	
	}
	
	//The help option
	if (std::string(argv[1]) == "-h" 
		|| std::string(argv[1]) == "--help") { 
		help();
	}
	
	//Holds options
	std::vector<std::string> inputs;
	std::string output = "a.out";	// -o
	bool obj_only = false;			// -c
	bool no_builtin = false;		// --no-builtin
	bool asm_only = false;			// -s, --asm-only
	bool dump_asm = false;			// -p, --print-asm
	bool no_asm = false;			// -a, --asm
	bool no_link = false;			// -l, --link
	bool verbose = false;			// -v, --verbose
	
	//Iterate through and collect options
	for (int i = 1; i<argc; i++) {
		auto option = std::string(argv[i]);
		
		if (option == "-o") {
		
		} else if (option == "-c") {
			obj_only = true;
		} else if (option == "--no-builtin") {
			no_builtin = true;
		} else if (option == "-s" || option == "--asm-only") {
			asm_only = true;
		} else if (option == "-p" || option == "--print-asm") {
			dump_asm = true;
		} else if (option == "-a" || option == "--asm") {
			no_asm = true;
		} else if (option == "-l" || option == "--link") {
			no_link = true;
		} else if (option == "-v" || option == "--verbose") {
			verbose = true;
			
		//Something else...
		} else {
			if (option[0] == '-') {
				std::cout << "Unknown option: " << option << std::endl;
				std::exit(1);
			} else {
				inputs.push_back(option);
			}
		}
	}
	
	//Iterate through each input and work on each file
	Asm_x86 builder;
	
	for (auto path : inputs) {
		auto lines = load_source(path.c_str());
		AstNode *node = build_ast(lines);
		
		builder.assemble(path, node);
		builder.write();
		
		delete node;
	}
	
	builder.build();
	
	return 0;
}
