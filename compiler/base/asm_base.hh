#pragma once

#include <string>
#include <fstream>

#include <ltac/ltac.hh>

class AsmGen {
public:
	AsmGen(LtacFile *f);
	void build_PIC() { pic = true; }
	void write();
protected:
	LtacFile *file;
	bool pic = false;
	std::ofstream writer;
	
	virtual void build_data(LtacDataSec *data) {}
	virtual void build_code(LtacCodeSec *code) {}
};
