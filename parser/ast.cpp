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
