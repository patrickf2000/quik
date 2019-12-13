#pragma once

#include <vector>
#include <string>

#include <lex.hh>

enum AstType {
	Global,
	Include,
	
	//Function stuff
	FuncDec,
	FuncCall,
	End,
	
	//Variable stuff
	VarDec,
	Id,
	Int
};

//The base of all our nodes
class AstNode {
public:
	explicit AstNode() {}
	explicit AstNode(AstType t) { type = t; }
	virtual ~AstNode() {}
	AstType type;
	std::vector<AstNode *> children;
};

//Represents a file to be included
class AstInclude : public AstNode {
public:
	explicit AstInclude() { type = AstType::Include; }
	explicit AstInclude(std::string path) {
		type = AstType::Include;
		include = path;
	}
	
	std::string get_include() { return include; }
	void set_include(std::string path) { include = path; }
private:
	std::string include;
};

//The function declaration type
//TODO: Arguments
class AstFuncDec : public AstNode {
public:
	explicit AstFuncDec() { type = AstType::FuncDec; }
	explicit AstFuncDec(std::string n) {
		type = AstType::FuncDec;
		name = n;
	}
	
	std::string get_name() { return name; }
	void set_name(std::string n) { name = n; }
private:
	std::string name = "";
};

//The function call type
class AstFuncCall : public AstNode {
public:
	explicit AstFuncCall() { type = AstType::FuncCall; }
	explicit AstFuncCall(std::string n, std::vector<Token> ar) {
		type = AstType::FuncCall;
		name = n;
		args = ar;
	}
	
	std::string get_name() { return name; }
	void set_name(std::string n) { name = n; }
	
	std::vector<Token> get_args() { return args; }
	void set_args(std::vector<Token> ar) { args = ar; }
private:
	std::string name;
	std::vector<Token> args;
};

//Variable declaration
//initial value go as children
class AstVarDec : public AstNode {
public:
	explicit AstVarDec() { type = AstType::VarDec; }
	explicit AstVarDec(std::string n) {
		type = AstType::VarDec;
		name = n;
	}
	
	std::string get_name() { return name; }
	void set_name(std::string n) { name = n; }
private:
	std::string name;
};

//The ID type
class AstID : public AstNode {
public:
	explicit AstID() { type = AstType::Id; }
	explicit AstID(std::string n) {
		type = AstType::Id;
		name = n;
	}
	
	std::string get_name() { return name; }
	void set_name(std::string n) { name = n; }
private:
	std::string name;
};

//The Integer type
class AstInt : public AstNode {
public:
	explicit AstInt() { type = AstType::Int; }
	explicit AstInt(int i) {
		type = AstType::Int;
		no = i;
	}
	
	int get_val() { return no; }
	void set_val(int i) { no = i; }
private:
	int no = 0;
};

//Debugging stuff
std::string ast2str(AstType type);
void print_tree(AstNode *node, int indent = 0);
