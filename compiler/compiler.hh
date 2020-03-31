#pragma once

#include <vector>
#include <string>

#include <ltac.hh>

#include "config.hh"

class Compiler {
public:
	explicit Compiler(Config c);
	void set_inputs(std::vector<std::string> inputs);
	void assemble();
	void compile();
	void link();
private:
	Config config;
	
	std::vector<std::string> src_files;
	std::vector<std::string> asm_files;
	std::vector<std::string> obj_files;
};
