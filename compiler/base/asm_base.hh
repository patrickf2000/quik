#pragma once

#include <string>
#include <fstream>

#include <ltac/ltac.hh>

class AsmGen {
public:
	AsmGen(LtacFile *f);
protected:
	LtacFile *file;
	std::ofstream writer;
};
