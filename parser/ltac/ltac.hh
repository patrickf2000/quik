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
enum class LtacCode {
	None,
	Func,
	Label
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
	LtacType node_type;
	std::vector<LtacNode *> children;
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

//Useful functions
void print_ltac(LtacFile *file);

