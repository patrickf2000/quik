#include <map>

#include "parse.hh"
#include "types.hh"

//Scans the tree for variable declarations
void QkParser::find_variables(AstNode *top) {
    if (top->getType() != AstType::Scope) {
        return;
    }
    
    AstScope *scope = static_cast<AstScope *>(top);

    for (auto node : top->getChildren()) {
        switch (node->getType()) {
            case AstType::Func: {
                AstFunc *fd = static_cast<AstFunc *>(node);
                AstScope *next = static_cast<AstScope *>(fd->getChildren().at(0));
                next->setVars(scope->getVars());
                
                for (auto v : fd->getArgs()) {
                    next->addVar(v);
                }
                
                find_variables(next);
            } break;
            
            case AstType::If:
            case AstType::Elif:
            case AstType::Else:
            case AstType::While: {
                AstScope *s = new AstScope;
                s->setVars(scope->getVars());
                s->addChildren(node->getChildren());
                
                find_variables(s);
                //scope->vars.insert(s->vars.begin(), s->vars.end());
                scope->addVars(s->getVars());
            } break;
            
            case AstType::ForEach: {
                AstScope *s = new AstScope;
                s->addChildren(node->getChildren());
                
                find_variables(s);
                //scope->vars.insert(s->vars.begin(), s->vars.end());
                scope->addVars(s->getVars());
                
                AstForEach *fe = static_cast<AstForEach *>(node);
                Var v;
                v.name = fe->i_var;
                v.is_param = false;
                v.is_array = false;
                scope->addVar(v);
            } break;
            
            case AstType::ArrayDec:
            case AstType::Var: {
                AstVar *vd = static_cast<AstVar *>(node);
                if (!vd->isDeclaration()) break;
                auto name = vd->getName();
                
                if (scope->varExists(name)) {
                    syntax_error(node->getLine(), "Duplicate variable declaration.");
                }
            
                Var v;
                v.name = name;
                v.type = vd->getDataType();
                v.is_param = false;
                scope->addVar(v);
            } break;
        }
    }
}

//Locates variable assignments and assigns the right type to them
// We also make sure there are no undeclared variables here
void QkParser::find_assign(AstNode *top, AstScope *scope) {
    for (auto node : top->getChildren()) {
        switch (node->getType()) {
            case AstType::Func: {
                auto child = node->getChildren().at(0);
                auto n_scope = static_cast<AstScope *>(child);
                find_assign(child, n_scope); 
            } break;
            
            case AstType::ArrayAssign:
            case AstType::ArrayAccess:
            case AstType::Var: {
                AstVar *va = static_cast<AstVar *>(node);
                if (va->isDeclaration()) break;
            
                Var v = scope->getVar(va->getName());
                va->setDataType(v.type);
            } break;
            default: {
                if (node->getChildren().size() > 0) {
                    find_assign(node, scope);
                }
            }
        }
    }
}

//Finds conditional nodes, and adds end-if blocks
void QkParser::find_cond(AstNode *top) {
    AstType last_type;
    auto children = top->getChildren();

    for (int i = 0; i < children.size(); i++) {
        auto node = children.at(i);
        
        if (node->getType() == AstType::Func) {
            AstScope *next = static_cast<AstScope *>(node->getChildren().at(0));
            find_cond(next);
        } else if (node->getType() == AstType::While) {
            find_cond(node);
        } else if (node->getType() == AstType::If || node->getType() == AstType::Elif || node->getType() == AstType::Else) {
            if (node->getType() == AstType::Elif && last_type != AstType::If) {
                syntax_error(node->getLine(), "Elif without previous If statement.");
            }
        
            if (node->getType() == AstType::If || node->getType() == AstType::Elif) {
                find_cond(node);
            }
        
            if (i+1 >= top->getChildren().size()) {
                AstNode *end = new AstNode(AstType::EndIf);
                //top->children.insert(top->getChildren().begin()+i+1, end);
                top->addChild(end, i+1);
                
                continue;
            }
            
            auto next = top->getChildren().at(i+1);
            if (next->getType() != AstType::Elif && next->getType() != AstType::Else) {
                AstNode *end = new AstNode(AstType::EndIf);
                //top->children.insert(top->getChildren().begin()+i+1, end);
                top->addChild(end, i+1);
            }
        }
        
        last_type = node->getType();
    }
}

//Scans the tree and adds empty return statements to
// functions that don't have any (needed for the backend)
void QkParser::check_return(AstNode *top) {
    for (auto node : top->getChildren()) {
        if (node->getType() == AstType::Func && node->getChildren().size() > 0) {
            auto scope = node->getChildren().at(0);
            auto children = scope->getChildren();
            auto last = children.at(children.size()-1);
            
            if (last->getType() != AstType::Return) {
                AstReturn *ret = new AstReturn;
                scope->addChild(ret);
            }
        }
    }
}
