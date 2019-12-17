#pragma once

#include <vector>
#include <string>
#include <map>

#include <lex.hh>

//Denotes the types of our nodes so we can upcast
enum AstType {
	Scope,
	Include,
	
	//Function stuff
	FuncDec,
	ExternFunc,
	FuncCall,
	End,
	Return,
	
	//Conditional stuff
	If,
	Elif,
	Else,
	
	//Variable stuff
	VarDec,
	VarAssign,
	Id,
	Int,
	Str,
	
	//Operator
	Add,
	Sub,
	Mul,
	Div,
	Mod
};

//Holds variable information
struct Var {
	std::string name;
	DataType type;
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

//The base class for nodes that have a string attribute
class AstAttrNode : public AstNode {
public:
	std::string get_name() { return name; }
	void set_name(std::string n) { name = n; }
protected:
	std::string name = "";
};

//Represents a scope
class AstScope : public AstNode {
public:
	AstScope() { type = AstType::Scope; }
	std::map<std::string, Var> vars;
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
class AstFuncDec : public AstAttrNode {
public:
	explicit AstFuncDec() { type = AstType::FuncDec; }
	explicit AstFuncDec(std::string n) {
		type = AstType::FuncDec;
		name = n;
	}

	std::vector<Var> args;
};

//Extern function declarations
class AstExternFunc : public AstFuncDec {
public:
	explicit AstExternFunc() { type = AstType::ExternFunc; }
	explicit AstExternFunc(std::string n) {
		type = AstType::ExternFunc;
		name = n;
	}
};

//The function call type
class AstFuncCall : public AstAttrNode {
public:
	explicit AstFuncCall() { type = AstType::FuncCall; }
	explicit AstFuncCall(std::string n) {
		type = AstType::FuncCall;
		name = n;
	}
};

//The return keyword
class AstReturn : public AstNode {
public:
	AstReturn() { type = AstType::Return; }
};

//The base class for conditionals
class AstCond : public AstNode {
public:
	CondOp get_op() { return op; }
	void set_op(CondOp o) { op = o; }
private:
	CondOp op;
};

//The If keyword
class AstIf : public AstCond {
public:
	AstIf() { type = AstType::If; }
};

class AstElif : public AstCond {
public:
	AstElif() { type = AstType::Elif; }
};

//The else keyword
class AstElse : public AstNode {
public:
	AstElse() { type = AstType::Else; }
};

//Variable declaration
//initial value go as children
class AstVarDec : public AstAttrNode {
public:
	explicit AstVarDec() { type = AstType::VarDec; }
	explicit AstVarDec(std::string n) {
		type = AstType::VarDec;
		name = n;
	}
	
	DataType get_type() { return dtype; }
	void set_type(DataType t) { dtype = t; }
private:
	DataType dtype;
};

//Variable assignment/operation
class AstVarAssign : public AstVarDec {
public:
	explicit AstVarAssign() { type = AstType::VarAssign; }
	explicit AstVarAssign(std::string n) {
		type = AstType::VarAssign;
		name = n;
	}
};

//The ID type
class AstID : public AstAttrNode {
public:
	explicit AstID() { type = AstType::Id; }
	explicit AstID(std::string n) {
		type = AstType::Id;
		name = n;
	}
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

//The string type
class AstString : public AstNode {
public:
	explicit AstString() { type = AstType::Str; }
	explicit AstString(std::string s) {
		type = AstType::Str;
		val = s;
	}
	
	std::string get_val() { return val; }
	void set_val(std::string s) { val = s; }
private:
	std::string val = "";
};

//Debugging stuff
std::string ast2str(AstType type);
void print_tree(AstNode *node, int indent = 0);
