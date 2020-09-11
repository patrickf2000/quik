#pragma once

#include <vector>
#include <string>
#include <map>
#include <sstream>

#include <lex.hh>

//Denotes the types of our nodes so we can upcast
enum AstType {
    Scope,
    
    //Function stuff
    FuncDec,
    ExternFunc,
    FuncCall,
    End,
    Return,
    
    //Structure stuff
    Struct,
    StructDec,
    StructAcc,
    StructMod,
    
    //Conditional stuff
    If,
    Elif,
    Else,
    EndIf,
    
    //Loop stuff
    While,
    Loop,
    ForEach,
    
    //Variable stuff
    VarDec,
    VarAssign,
    MultiVarAssign,
    Id,
    Int,
    Hex,
    Char,
    Bool,
    Float,
    Int64,
    Int128,
    Int256,
    Float64,
    Float80,
    Float128,
    Float256,
    Str,
    ArrayDec,
    ArrayAccess,
    ArrayAssign,
    Math,
    
    //Operator
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Inc,
    DMul,
    
    //Logical operators
    And,
    Or,
    Xor
};

//Holds variable information
struct Var {
    std::string name;
    DataType type;
    bool is_array;
    bool is_param;
    
    //Needed for the assembler
    int stack_pos;
    int size;
};

//The base of all our nodes
class AstNode {
public:
    explicit AstNode() {}
    explicit AstNode(AstType t) { type = t; }
    virtual ~AstNode() {}
    
    virtual std::string writeDot(std::string prefix = "");
    
    std::string writeDotStd(std::string prefix, std::string val, std::string color = "");
    std::string writeDotParent(std::string prefix, std::string nodeName, std::string shape = "");
    
    AstType type;
    std::vector<AstNode *> children;
    Line ln;
};

// The top-most AST node
class AstTree : public AstNode {
public:
    AstTree(std::string file) { this->file = file; }
    std::string writeDot();
private:
    std::string file;
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
class AstScope : public AstAttrNode {
public:
    AstScope() { type = AstType::Scope; }
    std::map<std::string, Var> vars;
    
    std::string writeDot(std::string prefix = "");
};

//The function declaration type
class AstFuncDec : public AstAttrNode {
public:
    explicit AstFuncDec() { type = AstType::FuncDec; }
    explicit AstFuncDec(std::string n) {
        type = AstType::FuncDec;
        name = n;
    }
    
    std::string writeDot(std::string prefix = "");

    std::vector<Var> args;
    bool is_global = false;
};

//Extern function declarations
class AstExternFunc : public AstFuncDec {
public:
    explicit AstExternFunc() { type = AstType::ExternFunc; }
    explicit AstExternFunc(std::string n) {
        type = AstType::ExternFunc;
        name = n;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
};

//The function call type
class AstFuncCall : public AstAttrNode {
public:
    explicit AstFuncCall() { type = AstType::FuncCall; }
    explicit AstFuncCall(std::string n) {
        type = AstType::FuncCall;
        name = n;
    }
    
    std::string writeDot(std::string prefix = "");
};

//The return keyword
class AstReturn : public AstNode {
public:
    AstReturn() { type = AstType::Return; }
    
    std::string writeDot(std::string prefix = "");
};

//Structure declarations
class AstStructDec : public AstAttrNode {
public:
    explicit AstStructDec() { type = AstType::StructDec; }
    explicit AstStructDec(std::string n) {
        type = AstType::StructDec;
        name = n;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
};

//A struct variable
class AstStruct : public AstNode {
public:
    explicit AstStruct() { type = AstType::Struct; }
    
    std::string writeDot(std::string prefix = "") { return ""; }
    
    std::string str_name = "";        //Refers to the structure being used
    std::string var_name = "";        //The name of our structure variable
};

//A struct access
class AstStructAcc : public AstStruct {
public:
    explicit AstStructAcc() { type = AstType::StructAcc; }
    std::string writeDot(std::string prefix = "") { return ""; }
};

//A struct modification
class AstStructMod : public AstStruct {
public:
    explicit AstStructMod() { type = AstType::StructMod; }
    std::string writeDot(std::string prefix = "") { return ""; }
};

//The base class for conditionals
class AstCond : public AstNode {
public:
    CondOp get_op() { return op; }
    void set_op(CondOp o) { op = o; }
    
    AstNode *lval;
    AstNode *rval;
private:
    CondOp op;
};

//The If keyword
class AstIf : public AstCond {
public:
    AstIf() { type = AstType::If; }
    std::string writeDot(std::string prefix = "") { return ""; }
};

class AstElif : public AstCond {
public:
    AstElif() { type = AstType::Elif; }
    std::string writeDot(std::string prefix = "") { return ""; }
};

//The else keyword
class AstElse : public AstNode {
public:
    AstElse() { type = AstType::Else; }
    std::string writeDot(std::string prefix = "") { return ""; }
};

//The while keyword
class AstWhile : public AstCond {
public:
    AstWhile() { type = AstType::While; }
    std::string writeDot(std::string prefix = "") { return ""; }
};

//The loop keyword
class AstLoop : public AstNode {
public:
    AstLoop() { type = AstType::Loop; }
    std::string writeDot(std::string prefix = "") { return ""; }
    
    AstNode *param;
    std::string i_var = "";
};

//The foreach keyword
class AstForEach : public AstAttrNode {
public:
    AstForEach() { type = AstType::ForEach; }
    std::string writeDot(std::string prefix = "") { return ""; }
    
    std::string i_var = "";        //Index
    std::string r_var = "";        //Range
    std::string i_var_in = "";    //Internal index counter
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
    
    std::string writeDot(std::string prefix = "");
    
    DataType get_type() { return dtype; }
    void set_type(DataType t) { dtype = t; }
protected:
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
    
    std::string writeDot(std::string prefix = "");
};

//Represents a math operation
class AstMath : public AstNode {
public:
    explicit AstMath() { type = AstType::Math; }
    std::string writeDot(std::string prefix = "");
};

//The ID type
class AstID : public AstAttrNode {
public:
    explicit AstID() { type = AstType::Id; }
    explicit AstID(std::string n) {
        type = AstType::Id;
        name = n;
    }
    
    std::string writeDot(std::string prefix = "");
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
    
    std::string writeDot(std::string prefix = "");
    
private:
    int no = 0;
};

//The hex type
class AstHex : public AstNode {
public:
    explicit AstHex() { type = AstType::Hex; }
    explicit AstHex(unsigned short b) {
        type = AstType::Hex;
        byte = b;
    }
    
    unsigned short get_val() { return byte; }
    void set_val(unsigned short b) { byte = b; }
    
    void set_val(std::string str) {
        std::stringstream ss(str);
        ss.flags(std::ios_base::hex);
        ss >> byte;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
    
private:
    unsigned short byte;
};

//The char type
class AstChar : public AstNode {
public:
    explicit AstChar() { type = AstType::Char; }
    explicit AstChar(char c) {
        type = AstType::Char;
        ch = c;
    }
    
    char get_val() { return ch; }
    void set_val(char c) { ch = c; }
    
    std::string writeDot(std::string prefix = "") { return ""; }
private:
    char ch = 0;
};

//The bool types
class AstBool : public AstNode {
public:
    explicit AstBool() { type = AstType::Bool; }
    explicit AstBool(bool b) {
        type = AstType::Bool;
        val = b;
    }
    
    bool get_val() { return val; }
    void set_val(bool b) { val = b; }
    
    std::string writeDot(std::string prefix = "") { return ""; }
private:
    bool val = false;
};

//The float type
class AstFloat : public AstNode {
public:
    explicit AstFloat() { type = AstType::Float; }
    explicit AstFloat(float n) {
        type = AstType::Float;
        no = n;
    }
    
    float get_val() { return no; }
    void set_val(float n) { no = n; }
    
    std::string writeDot(std::string prefix = "") { return ""; }
protected:
    float no = 0;
};

//The int-64 type
//Holds two integer types
class AstInt64 : public AstVarDec {
public:
    explicit AstInt64() { 
        type = AstType::Int64;
        dtype = DataType::Int64;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
};

//The int-128 type
//Tells the compiler to use SSE extensions (with integers)
class AstInt128 : public AstVarDec {
public:
    explicit AstInt128() { 
        type = AstType::Int128;
        dtype = DataType::Int128;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
};

//The int-256 type
//Tells the compiler to use AVX extensions (with integers)
class AstInt256 : public AstVarDec {
public:
    explicit AstInt256() {
        type = AstType::Int256;
        dtype = DataType::Int256;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
};

//The float-64 type
//Holds two floating-point values
class AstFloat64 : public AstVarDec {
public:
    explicit AstFloat64() { 
        type = AstType::Float64;
        dtype = DataType::Float64;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
};

//The float-80 type
//This tells the compiler to use the FPU unit, which handles
// 80-bit types (hence the name)
class AstFloat80: public AstFloat {
public:
    explicit AstFloat80() { type = AstType::Float80; }
    explicit AstFloat80(double n) {
        type = AstType::Float80;
        no = n;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
};

//The float-128 type
//Tells the compiler to use SSE extensions
class AstFloat128 : public AstVarDec {
public:
    explicit AstFloat128() { 
        type = AstType::Float128;
        dtype = DataType::Float128;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
};

//The float-256 type
//Tells the compiler to use AVX extensions
class AstFloat256 : public AstVarDec {
public:
    explicit AstFloat256() {
        type = AstType::Float256;
        dtype = DataType::Float256;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
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
    
    std::string writeDot(std::string prefix = "");
private:
    std::string val = "";
};

//Mutli-variable assignment
class AstMultiVarAssign : public AstNode {
public:
    explicit AstMultiVarAssign() { 
        type = AstType::MultiVarAssign;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
    
    std::vector<AstID *> vars;
};

//The array type
class AstArrayDec : public AstVarDec {
public:
    explicit AstArrayDec() { type = AstType::ArrayDec; }
    
    int get_size() { return size; }
    void set_size(int s) { size = s; }
    
    std::string writeDot(std::string prefix = "") { return ""; }
private:
    int size;
};

//Array access
class AstArrayAcc : public AstVarDec {
public:
    explicit AstArrayAcc() { type = AstType::ArrayAccess; }
    explicit AstArrayAcc(std::string n) {
        type = AstType::ArrayAccess;
        name = n;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
};

//Array assignment
class AstArrayAssign : public AstVarDec {
public:
    explicit AstArrayAssign() { type = AstType::ArrayAssign; }
    explicit AstArrayAssign(std::string n) {
        type = AstType::ArrayAssign;
        name = n;
    }
    
    std::string writeDot(std::string prefix = "") { return ""; }
    
    AstNode *index;
};

