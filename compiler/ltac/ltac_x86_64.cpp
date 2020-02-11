#include <iostream>
#include <fstream>
#include <cstdlib>

#include <ltac.hh>

#include "ltac_gen.hh"

//Generates code for the x86-64 platform
void LTAC_Generator::build_x86_64(AsmFile *file) {
	std::cout << "64-bit" << std::endl;
}

//Compile
void LTAC_Generator::compile_x86_64(std::string bin) {
	std::string as_ln = "as " + output + ".asm -o " + output + ".o";
	std::string link_ln = "gcc " + output + ".o -no-pie -o " + bin;
	
	system(as_ln.c_str());
	system(link_ln.c_str());
}
