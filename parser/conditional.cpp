#include "parse.hh"

//Builds a conditional statement
AstCondStm *QkParser::build_conditional(TokenType loopType) {
    // The first token should be a left parantheses
    auto token = getNext();
    
    if (token != TokenType::LEFT_PAREN) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Incorrect conditional syntax.");
    }
    
    token = getNext();
    
    //Create the right type
    AstCondStm *cond = new AstCondStm;
    AstCond *cmp = new AstCond;
    
    if (loopType == TokenType::IF) {
        cond = new AstIf;
    } else if (loopType == TokenType::ELIF) {
        cond = new AstElif;
    } else if (loopType == TokenType::WHILE) {
        cond = new AstWhile;
    }
    
    while (token != TokenType::NL && token != TokenType::NONE) {
        
        // First, check to see if we are within an inner math statement or function call
        if (token == TokenType::LEFT_PAREN) {
            token = getNext();
            
            AstMath *math = new AstMath;
            buildVarParts(math);
            cmp->addChild(math);
                
            token = getNext();
            continue;
            
        // Now, check on function calls
        } else if (token == TokenType::ID) {
            std::string name = getSVal();
            token = getNext();
            
            if (token == TokenType::LEFT_PAREN) {
                AstFuncCall *call = buildFuncCall(name);
                cmp->addChild(call);
                    
                token = getNext();
            } else {
                auto *id = new AstId(name);
                cmp->addChild(id);
            }
            
            continue;
        }
        
        //Types
        switch (token) {
            //Values
            case TokenType::NO: {
                int val = getIVal();
                AstInt *i = new AstInt(val);
                cmp->addChild(i);
            } break;
            
            case TokenType::CHAR: {
                auto val = getSVal();
                AstChar *ch = new AstChar(val[0]);
                cmp->addChild(ch);
            } break;
            
            //Booleans
            case TokenType::B_TRUE: {
                AstBool *bl = new AstBool(true);
                cmp->addChild(bl);
            } break;
            
            case TokenType::B_FALSE: {
                AstBool *bl = new AstBool(false);
                cmp->addChild(bl);
            } break;
        
            //Operators
            case TokenType::EQUALS: {
                cmp->addChild(new AstCmpOp(CmpType::Eq));
            } break;
            
            case TokenType::NOT_EQUAL: {
                cmp->addChild(new AstCmpOp(CmpType::Neq));
            } break;
            
            case TokenType::GREATER: {
                cmp->addChild(new AstCmpOp(CmpType::Gt));
            } break;
            
            case TokenType::GREATER_EQ: {
                cmp->addChild(new AstCmpOp(CmpType::Gte));
            } break;
            
            case TokenType::LESS: {
                cmp->addChild(new AstCmpOp(CmpType::Lt));
            } break;
            
            case TokenType::LESS_EQ: {
                cmp->addChild(new AstCmpOp(CmpType::Lte));
            } break;
        }
        
        token = getNext();
    }
    
    //Return
    return cond;
}
