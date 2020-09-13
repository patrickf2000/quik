#include <vector>
#include <ast.hh>

#include "ltac_build.hh"

std::map<std::string, bool> funcs;

//Checks to see if a portion of a tree has any function calls
bool LTAC_Builder::has_func_calls(AstNode *top) {
    bool found = false;

    for (auto node : top->children) {
        if (node->type == AstType::FuncCall)
            return true;
            
        if (node->children.size() > 0)
            found = has_func_calls(node);
    }
    
    return found;
}

//Builds a function
LtacFunc *LTAC_Builder::build_func(AstNode *node, bool is_extern) {
    auto fd = static_cast<AstFuncDec *>(node);
    
    //Check for overloading
    std::string fn_name = fd->get_name();
    
    if (funcs.find(fn_name) == funcs.end()) {        //Not found
        funcs[fn_name] = false;
    } else {
        funcs[fn_name] = true;
        
        std::string overload = fn_name;
        
        for (auto arg : fd->args) {
            switch (arg.type) {
                case DataType::Int: overload += "_INT"; break;
                case DataType::Str: overload += "_STR"; break;
                
                //TODO: add the rest
            }
        }
        
        fn_name = overload;
        funcs[overload] = false;
    }
    
    //Start building
    auto l_fd = new LtacFunc(fn_name);
    file->addCode(l_fd);
    l_fd->is_global = fd->is_global;
    
    //If its extern, build that
    if (is_extern) {
        l_fd->is_extern = true;
        return l_fd;
    }
    
    //Add the arguments
    for (auto v : fd->args) {
        inc_stack(v.type);
        v.stack_pos = stack_pos;
        vars[v.name] = v;
        
        auto var = new LtacVar;
        var->pos = stack_pos;
        var->d_type = v.type;
        
        l_fd->children.push_back(var);
    }
    
    return l_fd;
}

//Builds a function call
LtacFuncCall *LTAC_Builder::build_func_call(AstNode *node) {
    auto fc = static_cast<AstFuncCall *>(node);
    auto fn_name = fc->get_name();
    
    //First, we need to determine if we have an overloaded function
    bool overload = false;
    
    if (funcs[fn_name])
        overload = true;
    
    //Start building
    auto l_fc = new LtacFuncCall(fn_name);
    
    //Build the arguments
    for (auto arg : fc->children) {
        switch (arg->type) {
            //Push an integer
            case AstType::Int: {
                auto i = static_cast<AstInt *>(arg);
                auto li = new LtacInt(i->get_val());
                l_fc->children.push_back(li);
                
                if (overload) fn_name += "_INT";
            } break;
        
            //Push a string
            case AstType::Str: {
                auto lstr = build_string(arg);
                l_fc->children.push_back(lstr);
                
                if (overload) fn_name += "_STR";
            } break;
            
            //Push another variable
            case AstType::Id: {
                auto id = static_cast<AstID *>(arg);
                auto v = vars[id->get_name()];
                
                auto var = new LtacVar;
                var->pos = v.stack_pos;
                var->d_type = v.type;
                l_fc->children.push_back(var);
                
                if (overload) {
                    switch (v.type) {
                        case DataType::Int: fn_name += "_INT"; break;
                        case DataType::Str: fn_name += "_STR"; break;
                        
                        //TODO: Add rest
                    }
                }
            } break;
            
            //TODO: Add the rest
        }
    }
    
    //Assign the function name
    l_fc->name = fn_name;
    
    return l_fc;
}

//Builds a return statement
void LTAC_Builder::build_ret(AstNode *node) {
    auto rnode = new LtacNode(ltac::Ret);
    file->addCode(rnode);
    
    //Build any return statement values
    for (auto arg : node->children) {
        switch (arg->type) {
            //Variables
            case AstType::Id: {
                auto id = static_cast<AstID *>(arg);
                auto v = vars[id->get_name()];
                
                auto var = new LtacVar;
                var->pos = v.stack_pos;
                var->d_type = v.type;
                rnode->children.push_back(var);
            } break;
            
            //TODO: Add the rest
        }
    }
}



