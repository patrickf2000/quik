#pragma once

#include <string>

#include <ltac.hh>

class LTAC_Generator {
public:
	LTAC_Generator(std::string out) {
		output = out;
	}
	
	void build_x86_64(AsmFile *file);
	void compile_x86_64(std::string bin);
private:
	std::string output;
};
