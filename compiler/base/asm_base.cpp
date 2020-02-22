#include "asm_base.hh"

AsmGen::AsmGen(LtacFile *f) {
	file = f;
	writer = std::ofstream(file->name);
}
