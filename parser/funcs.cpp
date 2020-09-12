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
    
    // Check flags
    int flags = 0;
    if (isGlobal) flags |= Global;
    if (isExtern) flags |= Extern;

    // The next should be an ID
    std::string name = getSVal();
    
    if (token != TokenType::ID) {
        syntax_error(getLnNo(), getCurrentLn(),
            "No function name specified!");
    }
    
    // Create the object
    auto *fd = new AstFunc(name, flags);

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
                
                fd->addArg(v);
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
            fd->addArg(v);
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
            call->addChild(v_str);
        } else if (token == TokenType::NO) {
            AstInt *v_int = new AstInt(getIVal());
            call->addChild(v_int);
        } else if (token == TokenType::CHAR) {
            auto val = getSVal();
            AstChar *v_ch = new AstChar(val[0]);
            call->addChild(v_ch);
        } else if (token == TokenType::DEC) {
            AstFloat *v_flt = new AstFloat(getFloatL());
            call->addChild(v_flt);
        } else if (token == TokenType::B_TRUE) {
            AstBool *bl = new AstBool(true);
            call->addChild(bl);
        } else if (token == TokenType::B_FALSE) {
            AstBool *bl = new AstBool(false);
            call->addChild(bl);
        } else if (token == TokenType::CHAR) {
        
        } else if (token == TokenType::ID) {
            AstId *v_id = new AstId(getSVal());
            call->addChild(v_id);
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
                
                auto *id = new AstId(name);
                node->addChild(id);
            } break;
            
            case TokenType::NO: {
                int val = getIVal();
                
                auto *i = new AstInt(val);
                node->addChild(i);
            } break;
            
            case TokenType::DEC: {
                double val = getFloatL();
                
                auto *f = new AstFloat(val);
                node->addChild(f);
            } break;
        }
        
        token = getNext();
    }

    return node;
}
