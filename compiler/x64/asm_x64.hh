#pragma once

#include <string>
#include <fstream>

#include <ltac/ltac.hh>

class Asm_x64 {
public:
	Asm_x64(LtacFile *f);
	void write();
protected:
	void build_data(LtacDataSec *data);
	void build_code(LtacCodeSec *code);
	
	void build_func(LtacNode *node);
	void build_var(LtacNode *node);
private:
	LtacFile *file;
	std::ofstream writer;
};