#include "parse.hh"

//This performs common checking on variable declarations
// Note: The ID token is the current token when called from the main parse loop
AstVarDec *QkParser::buildVarDec(TokenType dataType) {
    std::string name = getSVal();
    auto token = getNext();
    
    if (token != TokenType::ASSIGN) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Invalid variable declaration.");
    }
    
    AstVarDec *vd = new AstVarDec(name);
    vd->set_type(ttype2dtype(dataType));
    return vd;
}

//Translates each part into tokens for variable declarations and assignments
void QkParser::buildVarParts(AstNode *vd) {
	bool array_dec = false;
    auto token = getNext();
    
    if (token == TokenType::NL) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Invalid variable assignment.");
    }

	while (token != TokenType::NL && token != TokenType::NONE) {

		if (token == TokenType::LEFT_PAREN) {
			token = getNext();
			
			AstMath *math = new AstMath;
            buildVarParts(math);
			
			vd->children.push_back(math);
		} else if (token == TokenType::ID) {
			std::string name = getSVal();
            token = getNext();
            
            // Function declaration
            if (token == TokenType::LEFT_PAREN) {
    			AstFuncCall *call = buildFuncCall(name);
    			vd->children.push_back(call);
                    
                token = getNext();
                
            // Array access
            } else if (token == TokenType::L_BRACKET) {
                token = getNext();
    			AstNode *index = new AstNode;
    			
    			switch (token) {
    				case TokenType::NO: {
    					int val = getIVal();
    					index = new AstInt(val);
    				} break;
    				
    				case TokenType::ID: {
                        auto val = getSVal();
                        index = new AstID(val); 
                    } break;
                    
    				default: syntax_error(getLnNo(), getCurrentLn(),
                                "You can only access array elements via integers.");
    			}
                
                token = getNext();
                if (token != TokenType::R_BRACKET) {
    				syntax_error(getLnNo(), getCurrentLn(),
                        "Invalid array access syntax.");
                }
    		
    			AstArrayAcc *acc = new AstArrayAcc(name);
    			acc->children.push_back(index);
    			vd->children.push_back(acc);
                
                token = getNext();
                
            // Structure access
            } else if (token == TokenType::DOT) {
                /*Token s_var = tokens[i];		//The structure variable
			    Token i_var = tokens[i+2];		//The struct sub variable
    			
    			Line l;
    			l.tokens = tokens;
    			
    			if (s_var.type != TokenType::ID)
    				syntax_error(l, "Invalid structure named.");
    				
    			if (i_var.type != TokenType::ID)
    				syntax_error(l, "Invalid structure item referenced.");
    				
    			AstStructAcc *str_acc = new AstStructAcc;
    			str_acc->str_name = s_var.id;
    			str_acc->var_name = i_var.id;
    			vd->children.push_back(str_acc);*/
            } else {
                AstID *id = new AstID(name);
				vd->children.push_back(id);
            }
            
			continue;
		} else if (token == TokenType::COMMA) {
			array_dec = true;
            token = getNext();
			continue;
		} else {
			if (token == TokenType::NO) {
				int no = getIVal();
				AstInt *i = new AstInt(no);
				vd->children.push_back(i);
			} else if (token == TokenType::CHAR) {
				auto val = getSVal();
				AstChar *ch = new AstChar(val[0]);
				vd->children.push_back(ch);
			} else if (token == TokenType::DEC) {
				double no = getFloatL();
				AstFloat *i = new AstFloat(no);
				vd->children.push_back(i);
			} else if (token == TokenType::HEX) {
				AstHex *hex = new AstHex;
				hex->set_val(getSVal());
				vd->children.push_back(hex);
			} else if (token == TokenType::B_TRUE) {
				AstBool *bl = new AstBool(true);
				vd->children.push_back(bl);
			} else if (token == TokenType::B_FALSE) {
				AstBool *bl = new AstBool(false);
				vd->children.push_back(bl);
			} else if (token == TokenType::STRING) {
                auto val = getSVal();
				AstString *i = new AstString(val);
				vd->children.push_back(i);
			
			//Math operators
			} else if (token == TokenType::PLUS) {
				vd->children.push_back(new AstNode(AstType::Add));
			} else if (token == TokenType::MINUS) {
				vd->children.push_back(new AstNode(AstType::Sub));
			} else if (token == TokenType::MUL) {
				vd->children.push_back(new AstNode(AstType::Mul));
			} else if (token == TokenType::DIV) {
				vd->children.push_back(new AstNode(AstType::Div));
			} else if (token == TokenType::MOD) {
				vd->children.push_back(new AstNode(AstType::Mod));
			} else if (token == TokenType::D_PLUS) {
				vd->children.push_back(new AstNode(AstType::Inc));
			} else if (token == TokenType::D_MUL) {
				vd->children.push_back(new AstNode(AstType::DMul));
				
			//Logical operators
			} else if (token == TokenType::AND) {
				vd->children.push_back(new AstNode(AstType::And));
			} else if (token == TokenType::OR) {
				vd->children.push_back(new AstNode(AstType::Or));
			} else if (token == TokenType::XOR) {
				vd->children.push_back(new AstNode(AstType::Xor));
			}
		}
        
        token = getNext();
	}
	
	//Check to see if it is a mathematical expression
	if (vd->children.size() > 1 && vd->type != AstType::Math
		&& !array_dec) {
		AstMath *math = new AstMath;
		math->children = vd->children;
		
		vd->children.clear();
		vd->children.push_back(math);
	}
}
