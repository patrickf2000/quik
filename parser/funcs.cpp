#include "parse.hh"

//Builds a variable declaration
AstFunc *QkParser::buildFuncDec(bool isGlobal, bool isExtern) {
    // First, perform a syntax check
    // If the function has the global or extern modifier, the func keyword should be next
    auto token = getNext();
    
    if (isGlobal || isExtern) {
        if (token != TokenType::FUNC_DEC) {
            syntax_error(getLnNo(), getCurrentLn(),
                "Invalid external or global function declaration.");
        }
        
        token = getNext();
    }

    // The next should be an ID
    std::string name = getSVal();
    
    if (token != TokenType::ID) {
        syntax_error(getLnNo(), getCurrentLn(),
            "No function name specified!");
    }
    
    // Create the object
    AstFuncDec *fd = new AstFunc(name);
    fd->is_global = isGlobal;
    
    if (isExtern)
        fd = new AstExternFunc(name);

    //Check for and load any arguments
    token = getNext();
    
    if (token == TokenType::LEFT_PAREN) {
        
        //Iterate through and build our info
        TokenType last;
        Var v;
        v.is_param = true;
        v.is_array = false;
        
        token = getNext();
        
        while (token != TokenType::RIGHT_PAREN && token != TokenType::NONE) {
        
            if (token == TokenType::COLON) {
                if (last != TokenType::ID) {
                    syntax_error(getLnNo(), getCurrentLn(),
                        "Invalid function parameter syntax");
                }
                
                last = token;
                token = getNext();
                continue;
            } else if (token == TokenType::COMMA) {
                if (v.type == DataType::None || last == TokenType::ID) {
                    syntax_error(getLnNo(), getCurrentLn(),
                        "No datatype specified in parameter");
                }
                
                fd->args.push_back(v);
                v.is_array = false;
                v.name = "";
                v.type = DataType::None;
            } else if (token == TokenType::ID) {
                v.name = getSVal();
            } else if (last == TokenType::COLON) {
                v.type = ttype2dtype(token);
                
                if (v.type == DataType::None) {
                    syntax_error(getLnNo(), getCurrentLn(),
                        "Invalid parameter syntax.");
                }
                
                last = token;
                token = getNext();
                
                if (token == TokenType::L_BRACKET) {
                    token = getNext();
                    if (token != TokenType::R_BRACKET) {
                        syntax_error(getLnNo(), getCurrentLn(),
                            "Invalid parameter syntax.");
                    }
                    
                    v.is_array = true;
                }
                
                continue;
            }
                
            last = token;
            token = getNext();
        }
        
        if (v.name != "" && v.type != DataType::None) {
            fd->args.push_back(v);
        }
    }

    return fd;
}

//Builds a function call
AstFuncCall *QkParser::buildFuncCall(std::string name) {
    AstFuncCall *call = new AstFuncCall(name);
    TokenType token = getNext();
    
    while (token != TokenType::RIGHT_PAREN && token != TokenType::NONE) {
        if (token == TokenType::STRING) {
            AstString *v_str = new AstString(getSVal());
            call->children.push_back(v_str);
        } else if (token == TokenType::NO) {
            AstInt *v_int = new AstInt(getIVal());
            call->children.push_back(v_int);
        } else if (token == TokenType::CHAR) {
            auto val = getSVal();
            AstChar *v_ch = new AstChar(val[0]);
            call->children.push_back(v_ch);
        } else if (token == TokenType::DEC) {
            AstFloat *v_flt = new AstFloat(getFloatL());
            call->children.push_back(v_flt);
        } else if (token == TokenType::B_TRUE) {
            AstBool *bl = new AstBool(true);
            call->children.push_back(bl);
        } else if (token == TokenType::B_FALSE) {
            AstBool *bl = new AstBool(false);
            call->children.push_back(bl);
        } else if (token == TokenType::CHAR) {
        
        } else if (token == TokenType::ID) {
            AstID *v_id = new AstID(getSVal());
            call->children.push_back(v_id);
        }
        
        token = getNext();
    }

    return call;
}

//Builds a return statement
AstReturn *QkParser::buildRet() {
    AstReturn *node = new AstReturn;
    auto token = getNext();
    
    while (token != TokenType::NL) {
        switch (token) {
            case TokenType::ID: {
                auto name = getSVal();
                
                AstID *id = new AstID(name);
                node->children.push_back(id);
            } break;
            
            case TokenType::NO: {
                int val = getIVal();
                
                AstInt *i = new AstInt(val);
                node->children.push_back(i);
            } break;
            
            case TokenType::DEC: {
                double val = getFloatL();
                
                AstFloat *f = new AstFloat(val);
                node->children.push_back(f);
            } break;
        }
        
        token = getNext();
    }

    return node;
}
