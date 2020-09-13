#pragma once

#include <string>
#include <fstream>

#include <ltac/ltac.hh>

class AsmGen {
public:
	AsmGen(LtacFile *f);
	void build_PIC() { pic = true; }
	void write(bool x86 = true);
protected:
	LtacFile *file;
	bool pic = false;
	std::ofstream writer;
	
	virtual void buildData(LtacFile *file) {}
	virtual void buildCode(LtacFile *file) {}
};
