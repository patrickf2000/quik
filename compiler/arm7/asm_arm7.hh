#pragma once

#include <vector>
#include <string>

#include <base/asm_base.hh>
#include <ltac/ltac.hh>

class Asm_Arm7 : public AsmGen {
public:
	Asm_Arm7(LtacFile *f) : AsmGen(f) {}
	
	void build_data(LtacDataSec *data);
	void build_code(LtacCodeSec *code);
	
	void build_func(LtacNode *node);
	void build_ret(LtacNode *node);
private:
	std::vector<std::string> str_labels;
};
