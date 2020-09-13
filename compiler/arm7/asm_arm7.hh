#pragma once

#include <vector>
#include <string>

#include <base/asm_base.hh>
#include <ltac/ltac.hh>

class Asm_Arm7 : public AsmGen {
public:
	Asm_Arm7(LtacFile *f) : AsmGen(f) {}
	
	void buildData(LtacFile *file);
	void buildCode(LtacFile *file);
	
	void build_func(LtacNode *node);
	void build_func_call(LtacNode *node);
	void build_ret(LtacNode *node);
	
	void build_var(LtacNode *node);
private:
	std::vector<std::string> str_labels;
};
