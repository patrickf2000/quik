#pragma once

#include <string>
#include <vector>
#include <map>

enum class ltac {
	//Label
	Label,
	Call,
	Ret,
	Extern,

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
	Ptr,
	
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
	explicit AsmNode() { n_type = AsmType::Node; }
	explicit AsmNode(AsmType t) { n_type = t; }

	ltac type;
	AsmType n_type = AsmType::Node;
	std::vector<AsmNode *> children;
	std::string val = "";
	
	void add(AsmNode *child) {
		children.push_back(child);
	}
	
	void add(ltac t) {
		AsmNode *n = new AsmNode;
		n->type = t;
		children.push_back(n);
	}
};

class AsmFile : public AsmNode {
public:
	explicit AsmFile() { n_type = AsmType::File; }
	explicit AsmFile(std::string f) {
		n_type = AsmType::File;
		file = f;
	}
	
	std::string file = "";
	std::map<std::string, std::string> str_pool;
};

//Represents a string
class AsmString : public AsmNode {
public:
	explicit AsmString() { n_type = AsmType::String; }
	explicit AsmString(std::string s) {
		n_type = AsmType::String;
		val = s;
	}
};

//The debug function
std::string ltac_translate(AsmNode *node);

