#include <iostream>
#include <fstream>
#include <cstdlib>

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
