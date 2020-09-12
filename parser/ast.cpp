#include "ast.hh"
#include "types.hh"

int id = 0;

std::string AstNode::writeDotStd(std::string prefix, std::string val, std::string color) {
    std::string name = "ID" + std::to_string(id);
    ++id;
    
    std::string content = name + "[";
    
    if (color != "")
        content += "color=" + color + ",";
    
    content += "label=\"" + val + "\"];\n";
    content += prefix + "->" + name + "\n";
    return content;
}

std::string AstNode::writeDotParent(std::string prefix, std::string nodeName, std::string shape) {
    std::string name = "NODE" + std::to_string(id);
    ++id;
    
    std::string content = name + "[";
    
    if (shape != "")
        content += "shape=" + shape + ",";
    
    content += "label=\"" + nodeName + "\"];\n";
    prefix = prefix + " -> " + name;
    content += prefix + ";";
    
    for (auto item : this->children) {
        content += item->writeDot(name);
    }
    
    return content;
}

void AstNode::addChild(AstNode *node) {
    children.push_back(node);
}

void AstNode::addChild(AstNode *node, int pos) {
    children.insert(children.begin()+pos, node);
}

void AstNode::addChildren(std::vector<AstNode *> list) {
    for (auto item : list)
        children.push_back(item);
}

//=============================================
// AstTree-> Represents the file itself

AstTree::AstTree(std::string file) {
    this->file = file;
    this->type = AstType::Tree;
}

std::string AstTree::writeDot() {
    std::string content = "digraph AST {";
    
    content += "tree[label=\"" + file + "\"];\n";
    
    for (auto item : this->children) {
        content += item->writeDot("tree");
    }
    
    content += "}";
    return content;
}

//============================================
// AstScope-> Represents a variable scope

void AstScope::addVar(Var v) {
    vars[v.name] = v;
}

void AstScope::setVars(std::map<std::string, Var> vars) {
    this->vars = vars;
}

Var AstScope::getVar(std::string name) {
    return this->vars[name];
}

std::map<std::string, Var> AstScope::getVars() {
    return vars;
}

bool AstScope::varExists(std::string name) {
    if (this->vars.find(name) != this->vars.end()) {
        return true;
    }
    return false;
}

std::string AstScope::writeDot(std::string prefix) {
    return writeDotParent(prefix, "Scope", "box");
}

//============================================
// AstExpr-> Represents an expression (usually math)

std::string AstExpr::writeDot(std::string prefix) {
    return writeDotParent(prefix, "Expr", "box");
}

//====================================================
// AstFunc-> Represents a function declaration

AstFunc::AstFunc() {
    type = AstType::Func;
}

AstFunc::AstFunc(std::string name) {
    type = AstType::FuncDec;
    this->name = name;
}

AstFunc::AstFunc(std::string name, DataType type, int tf) {
    this->type = AstType::Func;
    this->name = name;
    this->retType = type;
    this->typeFlags = tf;
}

void AstFunc::addVar(Var v) {
    this->vars.push_back(v);
}

void AstFunc::addVars(std::map<std::string, Var> newVars) {
    this->vars.insert(newVars.begin(), newVars.end());
}

std::vector<Var> AstFunc::getArgs() {
    return vars;
}

std::string AstFunc::writeDot(std::string prefix) {
    std::string lbl = this -> name;
    std::string typeStr = "";
    
    switch (this->retType) {
        case DataType::Void: typeStr = "void"; break;
        case DataType::Char: typeStr = "char"; break;
        case DataType::Int: typeStr = "int"; break;
        case DataType::Float: typeStr = "float"; break;
    }
    
    if (typeFlags & Extern) typeStr = "extern " + typeStr;
    if (typeFlags & Ptr) typeStr += "*";
    if (typeFlags & Ref) typeStr += "&";
    if (typeFlags & USigned) typeStr = "u" + typeStr;
    
    lbl += " (" + typeStr + ")";
    
    return writeDotParent(prefix, lbl);
}

//=================================================
// AstArgList -> Represents a list of arguments (generally from a function)

AstArgList::AstArgList() {
    type = AstType::ArgList;
}

std::string AstArgList::writeDot(std::string prefix) {
    return writeDotParent(prefix, "ArgList");
}

//=================================================
// AstArg -> Represents an argument

AstArg::AstArg(std::string name, DataType dt, int tf) {
    type = AstType::Arg;
    this->name = name;
    this->dataType = dt;
    this->typeFlags = tf;
}

void AstArg::setPtr(int ptrLevel) {
    this->ptrLevel = ptrLevel;
}

std::string AstArg::writeDot(std::string prefix) {
    std::string lbl = name;
    std::string typeStr = "";
    
    switch (this->dataType) {
        case DataType::Void: typeStr = "void"; break;
        case DataType::Char: typeStr = "char"; break;
        case DataType::Int: typeStr = "int"; break;
        case DataType::Float: typeStr = "float"; break;
    }
    
    for (int i = 0; i<ptrLevel; i++) typeStr = "*" + typeStr;
    
    if (typeFlags & Ref) typeStr += "&";
    if (typeFlags & USigned) typeStr = "u" + typeStr;
    
    lbl += " (" + typeStr + ")";
    
    return writeDotStd(prefix, lbl);
}

//=================================================
// AstVArg -> Represents a variadic argument

AstVArg::AstVArg() {
    type = AstType::VArg;
    this->dataType = DataType::Any;
}

std::string AstVArg::writeDot(std::string prefix) {
    return writeDotStd(prefix, "...");
}

//=================================================
// AstFuncCall-> Represents a function call

AstFuncCall::AstFuncCall(std::string name) {
    type = AstType::FuncCall;
    this->name = name;
}

std::string AstFuncCall::writeDot(std::string prefix) {
    std::string n = "FC" + std::to_string(id) + "_" + this->name;
    ++id;
    
    std::string content = n + "[label=\"" + this->name + "()\"];\n";
    content += prefix + " -> " + n + ";";
    
    for (auto item : this->children) {
        content += item->writeDot(n);
    }
    
    return content;
}

//=================================================
// AstRet -> Represents a return statement

AstReturn::AstReturn() {
    type = AstType::Ret;
}

std::string AstReturn::writeDot(std::string prefix) {
    if (this->children.size() == 0)
        return writeDotStd(prefix, "Ret");
    return writeDotParent(prefix, "Ret");
}

//=================================================
// AstVar -> Represents either a variable or a declaration

AstVar::AstVar(std::string name) {
    type = AstType::Var;
    this->name = name;
}

AstVar::AstVar(std::string name, bool dec) {
    type = AstType::Var;
    this->name = name;
    this->declaration = dec;
}

AstVar::AstVar(std::string name, bool dec, DataType dt) {
    type = AstType::Var;
    this->name = name;
    this->declaration = dec;
    this->dataType = dt;
}

AstVar::AstVar(std::string name, bool dec, DataType dt, int tf) {
    type = AstType::Var;
    this->name = name;
    this->declaration = dec;
    this->dataType = dt;
    this->typeFlags = tf;
}

bool AstVar::isDeclaration() {
    return declaration;
}

void AstVar::setDeclaration(bool dec) {
    this->declaration = dec;
}

int AstVar::ptrLevel() {
    return ptr;
}

void AstVar::setPtr(int ptr) {
    this->ptr = ptr;
}

DataType AstVar::getDataType() {
    return this->dataType;
}

void AstVar::setDataType(DataType dt) {
    this->dataType = dt;
}

int AstVar::getTypeFlags() {
    return this->typeFlags;
}

void AstVar::setTypeFlags(int tf) {
    this->typeFlags = tf;
}

std::string AstVar::getName() {
    return name;
}

void AstVar::setName(std::string name) {
    this->name = name;
}

std::string AstVar::writeDot(std::string prefix) {
    std::string lbl = this->name;
    if (this->declaration) lbl = "var " + lbl;
    
    std::string typeStr = "";
    
    switch (this->dataType) {
        case DataType::Void: typeStr = "void"; break;
        case DataType::Char: typeStr = "char"; break;
        case DataType::Int: typeStr = "int"; break;
        case DataType::Float: typeStr = "float"; break;
    }
    
    for (int i = 0; i<ptr; i++) typeStr = "*" + typeStr;
    if (typeFlags & USigned) typeStr = "u" + typeStr;
    
    lbl += " (" + typeStr + ")";

    return writeDotParent(prefix, lbl);
}

//=================================================
// AstArrayDec -> Represents an array declaration

AstArrayDec::AstArrayDec() {
    type = AstType::ArrayDec; 
}

AstArrayDec::AstArrayDec(int size) {
    type = AstType::ArrayDec;
    this->size = size;
}

AstArrayDec::AstArrayDec(int size, DataType dt) {
    type = AstType::ArrayDec;
    this->size = size;
    this->dataType = dt;
}
    
int AstArrayDec::getSize() {
    return size;
}

void AstArrayDec::setSize(int s) {
    this->size = s;
}

DataType AstVar::getDataType() {
    return this->dataType;
}

void AstVar::setDataType(DataType dt) {
    this->dataType = dt;
}
    
std::string AstArrayDec::writeDot(std::string prefix) {
    std::string lbl = "arr " + this->name;
    
    std::string typeStr = "";
    
    switch (this->dataType) {
        case DataType::Void: typeStr = "void"; break;
        case DataType::Char: typeStr = "char"; break;
        case DataType::Int: typeStr = "int"; break;
        case DataType::Float: typeStr = "float"; break;
    }
    
    lbl += " (" + typeStr + ")";

    return writeDotParent(prefix, lbl);
}

//=================================================
// AstCond -> Represents a conditional statement

AstCond::AstCond() {
    type = AstType::Cond;
}

std::string AstCond::writeDot(std::string prefix) {
    return writeDotParent(prefix, "Cond");
}

//=================================================
// AstCondStm -> Represents a generic conditional statement

AstCondStm::AstCondStm() {
    this->cond = new AstCond;
}

AstCondStm::AstCondStm(AstCond *cond) {
    this->cond = cond;
}

std::string AstCondStm::writeDot(std::string prefix) {
    std::string name = "COND" + std::to_string(id);
    ++id;
    
    std::string content = name + "[shape=invtriangle, label=" + condName + "];\n";
    prefix = prefix + " -> " + name;
    content += prefix + ";";
    
    content += cond->writeDot(name);
    
    for (auto item : this->children) {
        content += item->writeDot(name);
    }
    
    return content;
}

//=================================================
// AstElse -> Represents an else statement

AstElse::AstElse() {
    type = AstType::Else;
}

std::string AstElse::writeDot(std::string prefix) {
    return writeDotParent(prefix, "Else", "invtriangle");
}

//=================================================
// AstLoop -> For the loop statement

AstLoop::AstLoop() {
    type = AstType::Loop;
}

std::string AstLoop::writeDot(std::string prefix) {
    return writeDotParent(prefix, "Loop");
}

//=================================================
// AstForEach -> For the for-each statement

AstForEach::AstForEach() {
    type = AstType::ForEach;
}

std::string AstForEach::writeDot(std::string prefix) {
    return writeDotParent(prefix, "ForEach");
}

//=================================================

AstByte::AstByte(unsigned char val) {
    type = AstType::Byte;
    this->val = val;
}

std::string AstByte::writeDot(std::string prefix) {
    char buf[5];
    sscanf(buf, "%x", &this->val);
    
    return writeDotStd(prefix, std::to_string(val), "lightpink");
}

//=================================================
// AstChar -> Represents a character

AstChar::AstChar(char val) {
    type = AstType::Char;
    this->val = val;
}

std::string AstChar::writeDot(std::string prefix) {
    std::string sval = "";
    sval += val;
    
    return writeDotStd(prefix, sval, "lawngreen");
}

//=================================================
// AstOctal -> Represents an octal value

AstOctal::AstOctal(std::string val) {
    type = AstType::Octal;
    this->val = val;
}

std::string AstOctal::writeDot(std::string prefix) {
    return writeDotStd(prefix, val, "maroon");
}

//=================================================
// AstInt -> Represents an integer

AstInt::AstInt(int val) {
    type = AstType::Int;
    this->val = val;
}

std::string AstInt::writeDot(std::string prefix) {
    return writeDotStd(prefix, std::to_string(val), "deepskyblue");
}

//=================================================
// AstFloat -> Represents a floating-point value

AstFloat::AstFloat(float val) {
    type = AstType::Float;
    this->val = val;
}

std::string AstFloat::writeDot(std::string prefix) {
    return writeDotStd(prefix, std::to_string(val), "deeppink");
}

//=================================================
// AstId-> Represents an ID (ie, variable) value

AstId::AstId(std::string val) {
    type = AstType::Id;
    this->val = val;
}

std::string AstId::writeDot(std::string prefix) {
    return writeDotStd(prefix, val, "darkcyan");
}

//=================================================
// AstRef-> Represents a memory reference to a variable

AstRef::AstRef(std::string val) {
    type = AstType::Ref;
    this->val = val;
}

std::string AstRef::writeDot(std::string prefix) {
    std::string ln = "&" + val;
    return writeDotStd(prefix, ln, "brown");
}

//=================================================
// AstPtr-> Represents a pointer to memory location

AstPtr::AstPtr(std::string val, int ptrLevel) {
    type = AstType::Ptr;
    this->val = val;
    this->ptrLevel = ptrLevel;
}

std::string AstPtr::writeDot(std::string prefix) {
    std::string ln = "";
    for (int i = 0; i<ptrLevel; i++) ln += "*";
    
    ln += val;
    return writeDotStd(prefix, ln, "coral");
}

//=================================================
// AstString -> String value

AstString::AstString(std::string val) {
    type = AstType::String;
    
    val.pop_back();
    val.erase(val.begin());
    this->val = val;
}

std::string AstString::writeDot(std::string prefix) {
    return writeDotStd(prefix, val, "coral");
}

//=================================================
// AstMathOp -> Math operator

AstMathOp::AstMathOp(MathType op) {
    this->type = AstType::MathOp;
    this->op = op;
}

std::string AstMathOp::writeDot(std::string prefix) {
    std::string opStr = "";
    switch (op) {
        case MathType::Add: opStr = "+"; break;
        case MathType::Sub: opStr = "-"; break;
        case MathType::Mul: opStr = "*"; break;
        case MathType::Div: opStr = "/"; break;
        case MathType::Inc: opStr = "++"; break;
        case MathType::Dec: opStr = "--"; break;
        default: opStr = "Math-> ????"; break;
    }
    
    return writeDotStd(prefix, opStr, "green");
}

//=================================================
// AstCmpOp -> Comparison operator

AstCmpOp::AstCmpOp(CmpType op) {
    this->type = AstType::CmpOp;
    this->op = op;
}

std::string AstCmpOp::writeDot(std::string prefix) {
    std::string opStr = "";
    switch (op) {
        case CmpType::Eq: opStr = "=="; break;
        case CmpType::Neq: opStr = "!="; break;
        case CmpType::Gt: opStr = ">"; break;
        case CmpType::Lt: opStr = "<"; break;
        case CmpType::Gte: opStr = ">="; break;
        case CmpType::Lte: opStr = "<="; break;
        default: opStr = "Cmp-> ????"; break;
    }

    return writeDotStd(prefix, opStr, "goldenrod");
}

