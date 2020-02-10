#pragma once

#include <vector>
#include <string>

enum class A_Asm {
	//Label
	Label,
	Call,
	Ret,

	//Instructions
	Add,
	Sub,
	Mul,
	Div,
	Mov,
	Cmp,
	
	//Branch
	Jmp,
	Je,
	Jne,
	Jg,
	Jl,
	Jge,
	Jle,
	
	//Registers
	Reg,
	SpReg,
	BpReg,
	VReg,		// XMM on Intel
	VVReg,		// YMM on Intel
	
	//Memory
	Mem,
	
	//Other
	No,
	Str,
	
	//Pseudo-commands
	//These don't correspond directly to assembly commands;
	// rather, they are general commands for assembly that
	// is very platform-specific
	Setup,
	Leave
};

//Types
enum class AsmType {
	Node,
	File,
	String
};

//The class types
class AsmNode {
public:
	A_Asm type;
	AsmType n_type = AsmType::Node;
	std::vector<AsmNode *> children;
};

class AsmFile : public AsmNode {
public:
	explicit AsmFile() { n_type = AsmType::File; }
	explicit AsmFile(std::string f) {
		n_type = AsmType::File;
		file = f;
	}
	
	std::string file = "";
};

//Represents a string
class AsmString : public AsmNode {
public:
	explicit AsmString() { n_type = AsmType::String; }
	explicit AsmString(std::string s) {
		n_type = AsmType::String;
		val = s;
	}
	
	std::string val = "";
};

//The debug function
std::string aasm_translate(AsmNode *node);

