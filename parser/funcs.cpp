#include "parse.hh"

//Builds a variable declaration
AstFuncDec *QkParser::build_func_dec(Line ln) {
	auto tokens = ln.tokens;
	bool is_extern = false;
	bool is_global = false;

	//Perform checks
	if (tokens.at(0).type == TokenType::EXTERN) {
		is_extern = true;
		tokens.erase(tokens.begin());
	} else if (tokens.at(0).type == TokenType::GLOBAL) {
		is_global = true;
		tokens.erase(tokens.begin());
	}
	
	if (tokens.size() < 2) {
		syntax_error(ln, "Invalid size.");
	}

	Token id = tokens.at(1);
	if (id.type != TokenType::ID) {
		syntax_error(ln, "No function name specified!");
	}
	
	AstFuncDec *fd = new AstFuncDec(id.id);
	fd->is_global = is_global;
	
	if (is_extern)
		fd = new AstExternFunc(id.id);

	//Generic syntax check
	if (tokens.size() > 2) {
		//Make sure we start and end with parantheses
		auto t1 = tokens.at(2).type;
		auto t2 = tokens.at(tokens.size() - 1).type;
		
		if (t1 != TokenType::LEFT_PAREN || t2 != TokenType::RIGHT_PAREN) {
			syntax_error(ln, "Expected opening or closing parantheses.");
		}
		
		//Iterate through and build our info
		TokenType last;
		Var v;
		v.is_param = true;
		v.is_array = false;
		
		for (int i = 3; i<tokens.size(); i++) {
			auto t = tokens.at(i);
			
			if (t.type == TokenType::RIGHT_PAREN) {
				if (v.name != "" && v.type != DataType::None) {
					fd->args.push_back(v);
				}
				
				break;
			} else if (t.type == TokenType::COLON) {
				if (last != TokenType::ID) {
					syntax_error(ln, "Invalid parameter syntax");
				}
				
				last = t.type;
				continue;
			} else if (t.type == TokenType::COMMA) {
				if (v.type == DataType::None || last == TokenType::ID) {
					syntax_error(ln, "No datatype specified in parameter");
				}
				
				fd->args.push_back(v);
				v.is_array = false;
				v.name = "";
				v.type = DataType::None;
			} else if (t.type == TokenType::ID) {
				v.name = t.id;
			} else if (last == TokenType::COLON) {
				v.type = ttype2dtype(t.type);
				
				if (v.type == DataType::None) {
					syntax_error(ln, "Invalid parameter syntax.");
				}
				
				Token nt1 = tokens[i+1];
				Token nt2 = tokens[i+2];
				
				if (nt1.type == TokenType::L_BRACKET && 
					nt2.type == TokenType::R_BRACKET) {
					v.is_array = true;
					i += 2;
				}
			}
				
			last = t.type;
		}
	}

	return fd;
}

//Builds a function call
AstFuncCall *QkParser::build_func_call(Line ln) {
	auto tokens = ln.tokens;
	std::string name = tokens.at(0).id;
	AstFuncCall *call = new AstFuncCall(name);
	Token last;

	for (int i = 2; i<tokens.size(); i++) {
		auto t = tokens.at(i);
		last = t;

		if (t.type == TokenType::RIGHT_PAREN) {
			break;
		} else if (t.type == TokenType::COMMA) {
			continue;
		} else if (t.type == TokenType::STRING) {
			AstString *v_str = new AstString(t.id);
			call->children.push_back(v_str);
		} else if (t.type == TokenType::NO) {
			AstInt *v_int = new AstInt(std::stoi(t.id));
			call->children.push_back(v_int);
		} else if (t.type == TokenType::CHAR) {
			AstChar *v_ch = new AstChar(t.id[0]);
			call->children.push_back(v_ch);
		} else if (t.type == TokenType::DEC) {
			AstFloat *v_flt = new AstFloat(std::stod(t.id));
			call->children.push_back(v_flt);
		} else if (t.type == TokenType::B_TRUE) {
			AstBool *bl = new AstBool(true);
			call->children.push_back(bl);
		} else if (t.type == TokenType::B_FALSE) {
			AstBool *bl = new AstBool(false);
			call->children.push_back(bl);
		} else if (t.type == TokenType::CHAR) {
		
		} else if (t.type == TokenType::ID) {
			AstID *v_id = new AstID(t.id);
			call->children.push_back(v_id);
		}
	}

	if (last.type != TokenType::RIGHT_PAREN) {
		syntax_error(ln, "No closing token");
	}

	return call;
}

//Builds a return statement
AstReturn *QkParser::build_ret(Line ln) {
	auto tokens = ln.tokens;
	AstReturn *node = new AstReturn;
			
	if (tokens.size() > 2) {
		syntax_error(ln, "Too many arguments in return statement.");
	} else if (tokens.size() == 2) {
		auto t = tokens.at(1);
		
		switch (t.type) {
			case TokenType::ID: {
				AstID *id = new AstID(t.id);
				node->children.push_back(id);
			} break;
			
			case TokenType::NO: {
				int val = std::stoi(t.id);
				AstInt *i = new AstInt(val);
				node->children.push_back(i);
			} break;
			
			case TokenType::DEC: {
				double val = std::stod(t.id);
				AstFloat *f = new AstFloat(val);
				node->children.push_back(f);
			} break;
		}
	}

	return node;
}
