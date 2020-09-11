#include "parse.hh"

//This performs common checking on variable declarations
AstVarDec *QkParser::basic_var_dec(Line ln) {
	if (ln.tokens.size() < 4) {
		syntax_error(ln, "Missing elements in declaration!");
	}

	Token id = ln.tokens.at(1);
	Token ival = ln.tokens.at(3);

	if (id.type != TokenType::ID) {
		syntax_error(ln, "A name must be specified for a variable.");
	}

	if (ln.tokens.at(2).type != TokenType::ASSIGN) {
		syntax_error(ln, "Expected \'=\'");
	}

	AstVarDec *vd = new AstVarDec(id.id);
	return vd;
}

//Translates each part into tokens for variable declarations and assignments
void QkParser::build_var_parts(AstNode *vd, int start, std::vector<Token> tokens) {
    currentLn.tokens = tokens;
    currentIndex = 0;
    
	bool array_dec = false;

	for (int i = start; i<tokens.size(); i++) {
		Token t = tokens.at(i);

		if (tokens[i].type == TokenType::LEFT_PAREN) {
			std::vector<Token> sub_tokens;
			int layer = 1;
			
			for (int j = i+1; j<tokens.size(); j++) {
				if (tokens[j].type == TokenType::LEFT_PAREN) {
					++layer;
				} else if (tokens[j].type == TokenType::RIGHT_PAREN && layer > 1) {
					--layer;
				} else if (tokens[j].type == TokenType::RIGHT_PAREN) {
					i = j;
					break;
				}
				
				sub_tokens.push_back(tokens.at(j));
			}
			
			AstMath *math = new AstMath;
			build_var_parts(math, 0, sub_tokens);
			
			vd->children.push_back(math);
		} else if (tokens[i+1].type == TokenType::LEFT_PAREN && tokens[i].type == TokenType::ID) {
			std::vector<Token> sub_tokens;
            
            std::string name = tokens[i].id;
            currentIndex = i + 1;
			
			for (int j = i + 1; j<tokens.size(); j++) {
				sub_tokens.push_back(tokens.at(j));
			
				if (tokens[j].type == TokenType::RIGHT_PAREN) {
					i = j;
					break;
				}
			}
			
			Line l;
			l.tokens = sub_tokens;
			
			AstFuncCall *call = build_func_call(name);
			vd->children.push_back(call);
		} else if (tokens[i+1].type == TokenType::L_BRACKET) {
			auto index_t = tokens[i+2];
			auto rb = tokens[i+3];
			AstNode *index = new AstNode;
			
			Line l;
			l.tokens = tokens;
			
			if (rb.type != TokenType::R_BRACKET)
				syntax_error(l, "Invalid array access syntax.");
			
			switch (index_t.type) {
				case TokenType::NO: {
					int val = std::stoi(index_t.id);
					index = new AstInt(val);
				} break;
				
				case TokenType::ID: index = new AstID(index_t.id); break;
				default: syntax_error(l, "You can only access array elements via integers.");
			}
		
			AstArrayAcc *acc = new AstArrayAcc(t.id);
			acc->children.push_back(index);
			vd->children.push_back(acc);
			
			i += 3;
		} else if (tokens[i+1].type == TokenType::DOT) {
			Token s_var = tokens[i];		//The structure variable
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
			vd->children.push_back(str_acc);
			
			i += 2;
		} else if (t.type == TokenType::COMMA) {
			array_dec = true;
			continue;
		} else {
			if (t.type == TokenType::NO) {
				int no = std::stoi(t.id);
				AstInt *i = new AstInt(no);
				vd->children.push_back(i);
			} else if (t.type == TokenType::CHAR) {
				char c = t.id[0];
				AstChar *ch = new AstChar(c);
				vd->children.push_back(ch);
			} else if (t.type == TokenType::DEC) {
				double no = std::stod(t.id);
				AstFloat *i = new AstFloat(no);
				vd->children.push_back(i);
			} else if (t.type == TokenType::HEX) {
				AstHex *hex = new AstHex;
				hex->set_val(t.id);
				vd->children.push_back(hex);
			} else if (t.type == TokenType::B_TRUE) {
				AstBool *bl = new AstBool(true);
				vd->children.push_back(bl);
			} else if (t.type == TokenType::B_FALSE) {
				AstBool *bl = new AstBool(false);
				vd->children.push_back(bl);
			} else if (t.type == TokenType::STRING) {
				AstString *i = new AstString(t.id);
				vd->children.push_back(i);
			
			//Math operators
			} else if (t.type == TokenType::PLUS) {
				vd->children.push_back(new AstNode(AstType::Add));
			} else if (t.type == TokenType::MINUS) {
				vd->children.push_back(new AstNode(AstType::Sub));
			} else if (t.type == TokenType::MUL) {
				vd->children.push_back(new AstNode(AstType::Mul));
			} else if (t.type == TokenType::DIV) {
				vd->children.push_back(new AstNode(AstType::Div));
			} else if (t.type == TokenType::MOD) {
				vd->children.push_back(new AstNode(AstType::Mod));
			} else if (t.type == TokenType::D_PLUS) {
				vd->children.push_back(new AstNode(AstType::Inc));
			} else if (t.type == TokenType::D_MUL) {
				vd->children.push_back(new AstNode(AstType::DMul));
				
			//Logical operators
			} else if (t.type == TokenType::AND) {
				vd->children.push_back(new AstNode(AstType::And));
			} else if (t.type == TokenType::OR) {
				vd->children.push_back(new AstNode(AstType::Or));
			} else if (t.type == TokenType::XOR) {
				vd->children.push_back(new AstNode(AstType::Xor));
				
			//Other variables
			} else {
				AstID *i = new AstID(t.id);
				vd->children.push_back(i);
			}
		}
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
