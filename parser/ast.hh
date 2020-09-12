#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <map>

#include "types.hh"

enum {
    None = 1 << 0,
    Ptr = 1 << 1,
    Ref = 1 << 2,
    Signed = 1 << 3,
    USigned = 1 << 4,
    Extern = 1 << 5,
    Global = 1 << 6,
    Func = 1 << 7
};

enum class MathType {
    None,
    Add,
    Sub,
    Mul,
    Div,
    Inc,
    Dec
};

enum class CmpType {
    None,
    Eq,
    Neq,
    Gt,
    Lt,
    Gte,
    Lte
};

enum class AstType {
    None,
    Tree,
    Scope,
    Math,
    
    Func,
    End,
    ArgList,
    Arg,
    VArg,
    
    FuncCall,
    Return,
    
    Var,
    MultiVarAssign,
    ArrayDec,
    ArrayAssign,
    ArrayAccess,
    
    StructDec,
    
    Cond,
    While,
    If,
    Elif,
    Else,
    EndIf,
    Loop,
    ForEach,
    
    Byte,
    Char,
    Octal,
    Int,
    Bool,
    Float,
    String,
    Id,
    Ref,
    Ptr,
    
    MathOp,
    CmpOp
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

// The base of all nodes
class AstNode {
public:
    AstNode() {}
    AstNode(AstType type) {
        this->type = type;
    }
    
    virtual std::string writeDot(std::string prefix) {
        return "";
    }
    
    std::string writeDotStd(std::string prefix, std::string val, std::string color = "");
    std::string writeDotParent(std::string prefix, std::string nodeName, std::string shape = "");
    
    void addChild(AstNode *node);
    void addChild(AstNode *node, int pos);
    void addChildren(std::vector<AstNode *> list);
    
    std::vector<AstNode *> getChildren() {
        return children;
    }
    
    AstType getType() {
        return type;
    }
    
    void setLine(Line ln) {
        this->ln = ln;
    }
        
    Line getLine() {
        return this->ln;
    }
protected:
    AstType type;
    std::vector<AstNode *> children;
    Line ln;
};

// The top-most AST node
class AstTree : public AstNode {
public:
    AstTree(std::string file);
    std::string writeDot();
private:
    std::string file;
};

// Represents a scope
class AstScope : public AstNode {
public:
    AstScope() { type = AstType::Scope; }
    
    void addVar(Var v);
    void addVars(std::map<std::string, Var> newVars);
    void setVars(std::map<std::string, Var> vars);
    Var getVar(std::string name);
    std::map<std::string, Var> getVars();
    bool varExists(std::string name);
    
    std::string writeDot(std::string prefix);
private:
    std::map<std::string, Var> vars;
};

// Represents a math expression
class AstMath : public AstNode {
public:
    AstMath() { type = AstType::Math; }
    std::string writeDot(std::string prefix);
};

// Represents a function declaration
// TODO: I eventually want to use the types below instead of the Var list
class AstFunc : public AstNode {
public:
    explicit AstFunc();
    explicit AstFunc(std::string name);
    explicit AstFunc(std::string name, int tf);
    explicit AstFunc(std::string name, DataType type, int tf);
    
    void addArg(Var v);
    std::vector<Var> getArgs();
    
    std::string getName() {
        return name;
    }
    
    int getTypeFlags() {
        return typeFlags;
    }
    
    std::string writeDot(std::string prefix);
private:
    std::string name = "";
    DataType retType = DataType::Void;
    int typeFlags = 0;
    std::vector<Var> args;
};

// Represents an argument list
class AstArgList : public AstNode {
public:
    AstArgList();
    std::string writeDot(std::string prefix);
};

// Represents an argument
class AstArg : public AstNode {
public:
    explicit AstArg(std::string name, DataType dt, int tf);
    void setPtr(int ptrLevel);
    
    std::string getName() { return name; }
    DataType getDataType() { return dataType; }
    int getTypeFlags() { return typeFlags; }
    
    std::string writeDot(std::string prefix);
protected:
    explicit AstArg() {}
    std::string name = "";
    DataType dataType = DataType::None;
    int typeFlags = None;
    int ptrLevel = 0;
};

// Represents a variadic argument
class AstVArg : public AstArg {
public:
    AstVArg();
    std::string writeDot(std::string prefix);
};

// Represents a function call
class AstFuncCall : public AstNode {
public:
    explicit AstFuncCall(std::string name);
    std::string writeDot(std::string prefix);
    
    std::string getName() { return name; }
private:
    std::string name = "";
};

// Represents a return statement
class AstReturn : public AstNode {
public:
    AstReturn();
    std::string writeDot(std::string prefix);
};

// Represents a variable
class AstVar : public AstNode {
public:
    explicit AstVar(std::string name);
    explicit AstVar(std::string name, bool dec);
    explicit AstVar(std::string name, bool dec, DataType dt);
    explicit AstVar(std::string name, bool dec, DataType dt, int tf);
    
    bool isDeclaration();
    void setDeclaration(bool dec);
    
    int ptrLevel();
    void setPtr(int ptr);
    
    DataType getDataType();
    void setDataType(DataType dt);
    
    int getTypeFlags();
    void setTypeFlags(int tf);
    
    std::string getName();
    void setName(std::string name);
    
    std::string writeDot(std::string prefix);
private:
    std::string name = "";
    bool declaration = false;
    int ptr = 0;                        // Represents pointer level; if 0, variable is not a pointer
    DataType dataType = DataType::None;
    int typeFlags = None;
};

//The array type
class AstArrayDec : public AstNode {
public:
    explicit AstArrayDec();
    explicit AstArrayDec(int size);
    explicit AstArrayDec(int size, DataType dt);
    
    int getSize();
    void setSize(int s);
    
    DataType getDataType();
    void setDataType(DataType dt);
    
    std::string getName();
    void setName(std::string name);
    
    std::string writeDot(std::string prefix = "");
private:
    int size = 0;
    DataType dataType = DataType::None;
    std::string name = "";
};

//Array assignment
class AstArrayAssign : public AstNode {
public:
    explicit AstArrayAssign();
    explicit AstArrayAssign(std::string n);
    
    void setIndex(AstNode *index);
    AstNode *getIndex();
    
    std::string writeDot(std::string prefix);
private:
    std::string name = "";
    AstNode *index = nullptr;
};

// Represents a conditional statement
class AstCond : public AstNode {
public:
    explicit AstCond();
    std::string writeDot(std::string prefix);
};

// Represents a generic conditional-based statement
class AstCondStm : public AstNode {
public:
    explicit AstCondStm();
    explicit AstCondStm(AstCond *cond);
    std::string writeDot(std::string prefix);
protected:
    AstCond *cond;
    std::string condName = "";
};

// Represents a while loop
class AstWhile : public AstCondStm {
public:
    explicit AstWhile() {
        type = AstType::While;
        condName = "While";
    }
    
    explicit AstWhile(AstCond *cond) : AstCondStm(cond) {
        type = AstType::While;
        condName = "While";
    }
};

// Represents an if statement
class AstIf : public AstCondStm {
public:
    explicit AstIf() {
        type = AstType::If;
        condName = "If";
    }
    
    explicit AstIf(AstCond *cond) : AstCondStm(cond) {
        type = AstType::If;
        condName = "If";
    }
};

// Represents an elif statement
class AstElif : public AstCondStm {
public:
    explicit AstElif() {
        type = AstType::Elif;
        condName = "Elif";
    }
    
    explicit AstElif(AstCond *cond) : AstCondStm(cond) {
        type = AstType::Elif;
        condName = "Elif";
    }
};

// Represents an else statement
class AstElse : public AstNode {
public:
    explicit AstElse();
    std::string writeDot(std::string prefix);
};

//The loop keyword
class AstLoop : public AstNode {
public:
    AstLoop();
    
    void setParam(AstNode *param);
    AstNode *getParam();
    
    std::string writeDot(std::string prefix);
private:
    AstNode *param = nullptr;
    std::string i_var = "";
};

// Represents a for-each loop
class AstForEach : public AstNode {
public:
    AstForEach();
    std::string writeDot(std::string prefix = "");
    
    std::string i_var = "";        //Index
    std::string r_var = "";        //Range
    std::string i_var_in = "";    //Internal index counter
};

// Represents a byte literal (otherwise known as a hex value)
class AstByte : public AstNode {
public:
    explicit AstByte(unsigned char val);
    std::string writeDot(std::string prefix);
private:
    unsigned char val = 0;
};

// Represents a character literal
class AstChar : public AstNode {
public:
    explicit AstChar(char val);
    std::string writeDot(std::string prefix);
private:
    char val = 0;
};

// Represents an octal literal
class AstOctal : public AstNode {
public:
    explicit AstOctal(std::string val);
    std::string writeDot(std::string prefix);
private:
    std::string val = "";
};

// Represents an integer
class AstInt : public AstNode {
public:
    explicit AstInt(int val);
    std::string writeDot(std::string prefix);
    
    int getVal() { return val; }
private:
    int val = 0;
};

// Represents a boolean
class AstBool : public AstNode {
public:
    explicit AstBool(bool val);
    std::string writeDot(std::string prefix);
    
    bool getVal() { return val; }
private:
    bool val = false;
};

// Represents a float
class AstFloat : public AstNode {
public:
    explicit AstFloat(float val);
    std::string writeDot(std::string prefix);
private:
    float val = 0;
};

// Represents an ID
class AstId : public AstNode {
public:
    explicit AstId(std::string val);
    std::string writeDot(std::string prefix);
    
    std::string getVal() { return val; }
private:
    std::string val = "";
};

// Represents a memory reference
class AstRef : public AstNode {
public:
    explicit AstRef(std::string val);
    std::string writeDot(std::string prefix);
    
    std::string getVal() { return val; }
private:
    std::string val = "";
};

// Represents a pointer to memory
class AstPtr : public AstNode {
public:
    explicit AstPtr(std::string val, int ptrLevel);
    std::string writeDot(std::string prefix);
    
    std::string getVal() { return val; }
    int getPtr() { return ptrLevel; }
private:
    std::string val = "";
    int ptrLevel = 0;
};

// Represents a string
class AstString : public AstNode {
public:
    explicit AstString(std::string val);
    std::string writeDot(std::string prefix);
    
    std::string getVal() { return val; }
private:
    std::string val = "";
};

//Mutli-variable assignment
class AstMultiVarAssign : public AstNode {
public:
    explicit AstMultiVarAssign() { 
        type = AstType::MultiVarAssign;
    }
    
    void addVar(AstId *var);
    std::vector<AstId *> getVars();
    
    std::string writeDot(std::string prefix); 
private:
    std::vector<AstId *> vars;
};

// Represents a math operator
class AstMathOp : public AstNode {
public:
    explicit AstMathOp(MathType op);
    std::string writeDot(std::string prefix);
    
    MathType getOpType() { return op; }
private:
    MathType op = MathType::None;
};

// Represents a comparison operator
class AstCmpOp : public AstNode {
public:
    explicit AstCmpOp(CmpType op);
    std::string writeDot(std::string prefix);
private:
    CmpType op = CmpType::None;
};

