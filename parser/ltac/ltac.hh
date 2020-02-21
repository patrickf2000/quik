#pragma once

#include <vector>
#include <string>

#include <types.hh>

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
	FuncCall,
	Label,
	Ret,
	Var,
	
	Int,
	String
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

//Function calls
class LtacFuncCall : public LtacNode {
public:
	explicit LtacFuncCall() { type = ltac::FuncCall; }
	explicit LtacFuncCall(std::string n) {
		type = ltac::FuncCall;
		name = n;
	}
	
	std::string name = "";
};

//Variable assignment/declaration/operation
class LtacVar : public LtacNode {
public:
	explicit LtacVar() { type = ltac::Var; }
	int pos = 0;
	int size = 0;
	DataType d_type;
};

//Integers
class LtacInt : public LtacNode {
public:
	explicit LtacInt() { type = ltac::Int; }
	explicit LtacInt(int i) {
		type = ltac::Int;
		val = i;
	}
	
	int val = 0;
};

//Strings
class LtacString : public LtacNode {
public:
	explicit LtacString() { type = ltac::String; }
	explicit LtacString(std::string n, std::string v) {
		type = ltac::String;
		name = n;
		val = v;
	}
	
	std::string name = "";
	std::string val = "";
};

//Useful functions
void print_ltac(LtacFile *file);

