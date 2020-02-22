#pragma once

#include <base/asm_base.hh>
#include <ltac/ltac.hh>

class Asm_Arm7 : public AsmGen {
public:
	Asm_Arm7(LtacFile *f) : AsmGen(f) {}
};
