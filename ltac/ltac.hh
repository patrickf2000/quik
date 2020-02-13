#pragma once

#include <string>
#include <vector>

enum class LTAC {
	None,
	File,
	Section,

	//Functions
	Func,
	Label,
	Extern,
	Ret,
	Call,
	Setup,
	Leave,
	
	//Data comparison
	Mov,
	Cmp,
	
	//Branching
	Jmp,
	Je,
	Jne,
	Jg,
	Jl,
	Jge,
	Jle,
	
	//Math
	Add,
	Sub,
	Mul,
	Div,
	Mod,
	
	//Vector math
	VAdd,
	VSub,
	VMul,
	VDiv,
	
	//Registers
	Reg,
	ArgReg,
	SpReg,
	BpReg,
	WReg,
	
	//Memory
	Mem,
	Ptr,
	
	//Data
	Byte,
	Int,
	Float,
	String
};

//The base node
class LtacNode {
public:
	explicit LtacNode() {}
	explicit LtacNode(LTAC t) { type = t; }

	LTAC type = LTAC::None;
	std::string val;
	std::vector<LtacNode *> args;
};

//Represents a memory address
class LtacMem : public LtacNode {
public:
	explicit LtacMem() { type = LTAC::Mem; }
	
	int index = 0;
	int scale = 1;
};

//Represents a pointer to a memory location
class LtacPtr : public LtacNode {
public:
	explicit LtacPtr() { type = LTAC::Ptr; }
};

//Represents a variable
class LtacVar : public LtacNode {
public:
	void set_val(std::string s) { val = s; }
	void set_name(std::string n) { name = n; }
	
	std::string get_val() { return val; }
	std::string get_name() { return name; }
protected:
	std::string name = "";
};

//Represents a string
class LtacString : public LtacVar {
public:
	explicit LtacString() { type = LTAC::String; }
	explicit LtacString(std::string n, std::string s) {
		type = LTAC::String;
		name = n;
		val = s;
	}
};

//Represents an integer
class LtacInt : public LtacVar {
public:
	explicit LtacInt() { type = LTAC::Int; }
	explicit LtacInt(int i) {
		type = LTAC::Int;
		i_val = i;
	}
	
	void set_val(int i) { i_val = i; }
	int get_val() { return i_val; }
private:
	int i_val = 0;
};

//Represents a floating-point number
class LtacFloat : public LtacVar {
public:
	explicit LtacFloat() { type = LTAC::Float; }
	explicit LtacFloat(std::string n, float v) {
		type = LTAC::Float;
		name = n;
		f_val = v;
	}
	
	void set_val(float v) { f_val = v; }
	float get_val() { return f_val; }
private:
	float f_val = 0;
};

//Represents a file
class LtacFile : public LtacNode {
public:
	explicit LtacFile() { type = LTAC::File; }
	explicit LtacFile(std::string p) {
		type = LTAC::File;
		path = p;
	}
	
	std::string path = "";
	
	std::vector<LtacString *> str_pool;
	std::vector<LtacNode *> children;
};
