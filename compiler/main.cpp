#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include <ast.hh>
#include <build.hh>

#include "utils.hh"
#include "types.hh"
#include "x86/asmx86.hh"

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
	Config config;
	
	std::vector<std::string> inputs;
	config.out_name = "out";		// -o
	config.build_lib = false;		// --lib
									// -l
	
	//Iterate through and collect options
	for (int i = 1; i<argc; i++) {
		auto option = std::string(argv[i]);
		
		if (option == "-o") {
			config.out_name = std::string(argv[i+1]);
			i += 2;
		} else if (option == "--lib") {
			config.build_lib = true;
		} else if (option == "-l") {
			config.libs.push_back(std::string(argv[i+1]));
			i += 2;
			
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
	Asm_x86 builder(config);
	
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
