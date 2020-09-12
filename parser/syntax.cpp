#include <iostream>
#include <cstdlib>

#include "syntax.hh"
#include "parse.hh"

std::vector<Error> errors;

//Checks the global scope for invalid arguments
// Only function declarations and variable declarations are allowed
// on the global scope
void SyntaxCheck::check_global(AstNode *top) {
    if (top->getType() != AstType::Scope) {
        return;
    }
    
    for (auto node : top->getChildren()) {
        switch (node->getType()) {
            case AstType::Func:
            case AstType::Var: 
            case AstType::EndIf: 
            case AstType::StructDec: continue;
        }
            
        Error er;
        er.ln = node->getLine();
        er.msg = "Invalid token on the global scope.";
        errors.push_back(er);
    }
}

//Scans from all the scopes, and makes sure there are no undeclared variables
void SyntaxCheck::check_vars(AstNode *top, std::map<std::string, Var> vars) {
    if (top->getType() == AstType::Scope) {
        AstScope *scope = static_cast<AstScope *>(top);
        vars = scope->getVars();
    }
    
    for (auto node : top->getChildren()) {
        if (node->getType() == AstType::VarAssign || node->getType() == AstType::Id) {
            std::string name = "";
            if (node->getType() == AstType::Var)
                name = static_cast<AstVar *>(node)->getName();
            else if (node->getType() == AstType::Id)
                name = static_cast<AstId *>(node)->getVak();
                
            if (!scope->varExists(name)) {
                Error er;
                er.ln = node->ln;
                er.msg = "Use of undeclared variable: " + va->get_name();
                errors.push_back(er);
            }
        }
        
        if (node->getChildren().size() > 0) {
            check_vars(node, vars);
        }
    }
}

//Scans the tree to check for invalid variable types in the loop keyword
void SyntaxCheck::check_lp_vars(AstNode *top, std::map<std::string, Var> vars) {
    if (top->getType() == AstType::Scope) {
        AstScope *scope = static_cast<AstScope *>(top);
        vars = scope->vars;
    }
    
    for (auto node : top->children) {
        if (node->getType() == AstType::Loop) {
            AstLoop *lp = static_cast<AstLoop *>(node);
            
            if (lp->param->getType() == AstType::Id) {
                AstAttrNode *va = static_cast<AstAttrNode *>(lp->param);
                
                if (vars.find(va->get_name()) == vars.end()) {
                    Error er;
                    er.ln = node->ln;
                    er.msg = "Use of undeclared variable.";
                    errors.push_back(er);
                } else {
                    Var v = vars[va->get_name()];
                    if (v.getType() != DataType::Int) {
                        Error er;
                        er.ln = node->ln;
                        er.msg = "Only integer variables may be used with the loop statement.";
                        errors.push_back(er);
                    }
                }
            }
        }
        
        if (node->children.size() > 0) {
            check_lp_vars(node, vars);
        }
    }
}

//Checks to see if we have any errors and prints them
void SyntaxCheck::evaluate(bool fail) {
    if (errors.size() == 0)
        return;
        
    for (auto err : errors) {
        std::cout << "Syntax Error: " << err.msg << std::endl;
        std::cout << "[" << err.ln.no << "] " << err.ln.original << std::endl;
        std::cout << std::endl;
    }
    
    if (fail)
        std::exit(1);
}
