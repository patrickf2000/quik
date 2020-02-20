#pragma once

#include <vector>
#include <string>

// This relates to the overall node types
enum class LtacType {
	None,
	File,
	Data,
	Code
};

// This contains the actual instructions
enum class ltac {
	None,
	Func,
	Label,
	Ret
};

// This relates to ltac data
enum class LtacData {
	None,
	String,
	Float,
	Int
};

//The nodes for ltac
//This is the base class for all nodes
class LtacNode {
public:
	explicit LtacNode() {}
	explicit LtacNode(ltac t) { type = t; }

	LtacType node_type;
	ltac type = ltac::None;		//Because most nodes are code/instruction nodes, we want this
	std::vector<LtacNode *> children;
};

//The base class for the data section
class LtacDataSec : public LtacNode {
public:
	explicit LtacDataSec() { node_type = LtacType::Data; }
};

//The base class for the for code section
class LtacCodeSec : public LtacNode {
public:
	explicit LtacCodeSec() { node_type = LtacType::Code; }
};

//This is the base class for the file
class LtacFile : public LtacNode {
public:
	explicit LtacFile() {
		node_type = LtacType::File;
	}
	
	explicit LtacFile(std::string n) {
		node_type = LtacType::File;
		name = n;
	}
	
	std::string name = "";
	
	LtacDataSec *data;
	LtacCodeSec *code;
};

//Functions
class LtacFunc : public LtacNode {
public:
	explicit LtacFunc() { type = ltac::Func; }
	explicit LtacFunc(std::string n) {
		type = ltac::Func;
		name = n;
	}
	
	std::string name = "";
};

//Useful functions
void print_ltac(LtacFile *file);

