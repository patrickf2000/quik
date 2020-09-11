#include <ast.hh>

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

std::string AstNode::writeDot(std::string prefix) {
    std::string opStr = "";
    switch (type) {
        case AstType::Add: opStr = "+"; break;
        case AstType::Sub: opStr = "-"; break;
        case AstType::Mul: opStr = "*"; break;
        case AstType::Div: opStr = "/"; break;
        case AstType::Inc: opStr = "++"; break;
        case AstType::DMul: opStr = "--"; break;
    }
    
    return writeDotStd(prefix, opStr, "green");
}

//=============================================
// AstTree-> Represents the file itself

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

std::string AstScope::writeDot(std::string prefix) {
    return writeDotParent(prefix, "Scope", "box");
}

//============================================
// AstMath-> Represents a math expression

std::string AstMath::writeDot(std::string prefix) {
    return writeDotParent(prefix, "Math", "box");
}

//====================================================
// AstFuncDec-> Represents a function declaration
// TODO: We need function arguments

std::string AstFuncDec::writeDot(std::string prefix) {
    std::string lbl = "func " + this->name;
    return writeDotParent(prefix, lbl);
}

//=================================================
// AstFuncCall-> Represents a function call

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
// AstReturn -> Represents a return statement

std::string AstReturn::writeDot(std::string prefix) {
    if (this->children.size() == 0)
        return writeDotStd(prefix, "Ret");
    return writeDotParent(prefix, "Ret");
}

//=================================================
// AstVarDec-> Represents a variable declaration

std::string AstVarDec::writeDot(std::string prefix) {
    std::string lbl = "var " + this->name;
    
    std::string typeStr = "";
    
    switch (this->dtype) {
        case DataType::Void: typeStr = "void"; break;
        case DataType::Char: typeStr = "char"; break;
        case DataType::Int: typeStr = "int"; break;
        case DataType::Float: typeStr = "float"; break;
    }
    
    //for (int i = 0; i<ptr; i++) typeStr = "*" + typeStr;
    //if (typeFlags & USigned) typeStr = "u" + typeStr;
    
    lbl += " (" + typeStr + ")";

    return writeDotParent(prefix, lbl);
}

//=================================================
// AstVarAssign-> Represents a variable declaration

std::string AstVarAssign::writeDot(std::string prefix) {
    std::string lbl = this->name;
    return writeDotParent(prefix, lbl);
}

//=================================================
// AstId-> Represents an ID (ie, variable) value

std::string AstID::writeDot(std::string prefix) {
    return writeDotStd(prefix, name, "darkcyan");
}

//=================================================
// AstInt -> Represents an integer

std::string AstInt::writeDot(std::string prefix) {
    return writeDotStd(prefix, std::to_string(no), "deepskyblue");
}

//=================================================
// AstString -> String value

std::string AstString::writeDot(std::string prefix) {
    return writeDotStd(prefix, val, "coral");
}
