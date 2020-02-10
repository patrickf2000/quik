#pragma once

#include <vector>
#include <string>

enum class A_Asm {
	//Label
	Label,

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
	Mem
};

//Types
enum class AsmType {
	Node,
	File
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
