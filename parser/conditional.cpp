#include "parse.hh"

//Builds a conditional statement
AstCond *QkParser::build_conditional(TokenType loopType) {
    // The first token should be a left parantheses
    auto token = getNext();
    
    if (token != TokenType::LEFT_PAREN) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Incorrect conditional syntax.");
    }
    
    token = getNext();
	
	//Create the right type
	AstCond *cond = new AstCond;
	
	if (loopType == TokenType::IF) {
		cond = new AstIf;
	} else if (loopType == TokenType::ELIF) {
		cond = new AstElif;
	} else if (loopType == TokenType::WHILE) {
		cond = new AstWhile;
	}
	
	//By default, make the operator equal and rval true
	cond->set_op(CondOp::Equals);
	cond->rval = new AstBool(true);
	
	//Parse the expression
	bool found_op = false;
	
	while (token != TokenType::NL && token != TokenType::NONE) {
		
        // First, check to see if we are within an inner math statement or function call
		if (token == TokenType::LEFT_PAREN) {
            token = getNext();
			std::vector<Token> sub_tokens;
			int layer = 1;
			
			while (token != TokenType::RIGHT_PAREN && layer > 1) {
				if (token == TokenType::LEFT_PAREN) {
					++layer;
				} else if (token == TokenType::RIGHT_PAREN && layer > 1) {
					--layer;
				} else if (token == TokenType::RIGHT_PAREN) {
					break;
				}
				
                Token t;
                t.type = token;
                t.id = getSVal();
				sub_tokens.push_back(t);
			}
			
			AstMath *math = new AstMath;
			build_var_parts(math, 0, sub_tokens);
			
			if (found_op)
				cond->rval = math;
			else
				cond->lval = math;
                
            token = getNext();
			continue;
            
        // Now, check on function calls
		} else if (token == TokenType::ID) {
            std::string name = getSVal();
            token = getNext();
            
            if (token == TokenType::LEFT_PAREN) {
    			AstFuncCall *call = build_func_call(name);
    			if (found_op)
    				cond->rval = call;
    			else
    				cond->lval = call;
                    
                token = getNext();
            } else {
                AstID *id = new AstID(name);
				
				if (found_op)
					cond->rval = id;
				else
					cond->lval = id;
            }
            
			continue;
		}
		
        //Types
		switch (token) {
			//Values
			case TokenType::NO: {
                int val = getIVal();
				AstInt *i = new AstInt(val);
				
				if (found_op)
					cond->rval = i;
				else
					cond->lval = i;
			} break;
			
			case TokenType::CHAR: {
                auto val = getSVal();
				AstChar *ch = new AstChar(val[0]);
				
				if (found_op)
					cond->rval = ch;
				else
					cond->lval = ch;
			} break;
			
			//Booleans
			case TokenType::B_TRUE: {
				AstBool *bl = new AstBool(true);
				
				if (found_op)
					cond->rval = bl;
				else
					cond->lval = bl;
			} break;
			
			case TokenType::B_FALSE: {
				AstBool *bl = new AstBool(false);
				
				if (found_op)
					cond->rval = bl;
				else
					cond->lval = bl;
			} break;
		
			//Operators
			case TokenType::EQUALS: {
				cond->set_op(CondOp::Equals);
				found_op = true;
			} break;
			
			case TokenType::NOT_EQUAL: {
				cond->set_op(CondOp::NotEquals);
				found_op = true;
			} break;
			
			case TokenType::GREATER: {
				cond->set_op(CondOp::Greater);
				found_op = true;
			} break;
			
			case TokenType::GREATER_EQ: {
				cond->set_op(CondOp::GreaterEq);
				found_op = true;
			} break;
			
			case TokenType::LESS: {
				cond->set_op(CondOp::Less);
				found_op = true;
			} break;
			
			case TokenType::LESS_EQ: {
				cond->set_op(CondOp::LessEq);
				found_op = true;
			} break;
		}
        
        token = getNext();
	}
	
	//Return
	return cond;
}
