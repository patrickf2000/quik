#include <iostream>
#include <cstdlib>

#include "parse.hh"
#include "types.hh"
#include "syntax.hh"

//Throws a syntax error and exits the program
void syntax_error(Line ln, std::string msg, bool exit) {
	Error e;
	e.ln = ln;
	e.msg = msg;
	errors.push_back(e);
}

//Translates a token type to a datatype
DataType ttype2dtype(TokenType t) {
	switch (t) {
		case TokenType::T_BYTE: return DataType::Byte;
		case TokenType::T_CHAR: return DataType::Char;
		case TokenType::T_SHORT: return DataType::Short;
		case TokenType::T_INT: return DataType::Int;
		case TokenType::T_FLOAT: return DataType::Float;
		case TokenType::T_BOOL: return DataType::Bool;
		case TokenType::T_STR: return DataType::Str;
		
		case TokenType::INT_128: return DataType::Int128;
		case TokenType::INT_256: return DataType::Int256;
		case TokenType::FLOAT_80: return DataType::Float80;
		case TokenType::FLOAT_128: return DataType::Float128;
		case TokenType::FLOAT_256: return DataType::Float256;
	}
	
	return DataType::None;
}

//Builds a function call
AstFuncCall *build_func_call(Line ln) {
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

//This performs common checking on variable declarations
AstVarDec *basic_var_dec(Line ln) {
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
void build_var_parts(AstNode *vd, int start, std::vector<Token> tokens) {
	for (int i = start; i<tokens.size(); i++) {
		Token t = tokens.at(i);
		
		if (tokens[i+1].type == TokenType::LEFT_PAREN) {
			std::vector<Token> sub_tokens;
			
			for (int j = i; j<tokens.size(); j++) {
				sub_tokens.push_back(tokens.at(j));
			
				if (tokens[j].type == TokenType::RIGHT_PAREN) {
					i = j;
					break;
				}
			}
			
			Line l;
			l.tokens = sub_tokens;
			
			AstFuncCall *call = build_func_call(l);
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
		} else if (t.type == TokenType::COMMA) {
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
			} else if (t.type == TokenType::B_TRUE) {
				AstBool *bl = new AstBool(true);
				vd->children.push_back(bl);
			} else if (t.type == TokenType::B_FALSE) {
				AstBool *bl = new AstBool(false);
				vd->children.push_back(bl);
			} else if (t.type == TokenType::STRING) {
				AstString *i = new AstString(t.id);
				vd->children.push_back(i);
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
			} else {
				AstID *i = new AstID(t.id);
				vd->children.push_back(i);
			}
		}
	}
}

//Builds a variable declaration
//This is separate because the extern token requires it, with only
// minor changes.
AstFuncDec *build_func_dec(Line ln) {
	auto tokens = ln.tokens;
	bool is_extern = false;

	//Perform checks
	if (tokens.at(0).type == TokenType::EXTERN) {
		is_extern = true;
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

//Build an array node
AstArrayDec *build_array(Line ln, bool is_float) {
	auto tokens = ln.tokens;
	auto type = tokens.at(0).type;

	AstArrayDec *arr = new AstArrayDec;
	arr->set_type(ttype2dtype(type));
	
	//Syntax checking
	auto lb = tokens.at(1).type;
	auto count = tokens.at(2);
	auto rb = tokens.at(3).type;
	auto name = tokens.at(4);
	
	if (lb != TokenType::L_BRACKET || rb != TokenType::R_BRACKET)
		syntax_error(ln, "Invalid array declaration syntax.");
		
	if (count.type != TokenType::NO)
		syntax_error(ln, "No or invalid size specified for array.");
		
	//Set the name and size
	arr->set_name(name.id);
	arr->set_size(std::stoi(count.id));
	
	//Set the elements
	for (int i = 6; i<tokens.size(); i++) {
		auto t = tokens.at(i);
		
		switch (t.type) {
			case TokenType::NO: {
				int no = std::stoi(t.id);
				AstInt *i = new AstInt(no);
				arr->children.push_back(i);
			} break;
			
			case TokenType::DEC: {
				double no = std::stod(t.id);
				AstFloat *i = new AstFloat(no);
				arr->children.push_back(i);
			} break;
			
			case TokenType::B_TRUE: {
				AstBool *bl = new AstBool(true);
				arr->children.push_back(bl);
			} break;	
			
			case TokenType::B_FALSE: {
				AstBool *bl = new AstBool(false);
				arr->children.push_back(bl);
				
			} break;
			
			case TokenType::STRING: {
				AstString *i = new AstString(t.id);
				arr->children.push_back(i);
			} break;
			
			case TokenType::ID: {
				AstID *id = new AstID(t.id);
				arr->children.push_back(id);
			} break;
			
			case TokenType::COMMA: continue;
		}
	}
	
	return arr;
}

//Builds a conditional statement
AstCond *build_conditional(Line ln) {
	auto tokens = ln.tokens;
	auto type = tokens.at(0).type;
	
	//Make sure the syntax is correct
	auto t1 = tokens.at(1).type;
	auto t2 = tokens.at(tokens.size() - 1).type;
	
	if (t1 != TokenType::LEFT_PAREN || t2 != TokenType::RIGHT_PAREN) {
		syntax_error(ln, "Incorrect conditional syntax.");
	}
	
	//Create the right type
	AstCond *cond = new AstCond;
	
	if (type == TokenType::IF) {
		cond = new AstIf;
	} else if (type == TokenType::ELIF) {
		cond = new AstElif;
	} else if (type == TokenType::WHILE) {
		cond = new AstWhile;
	}
	
	//By default, make the operator equal and rval true
	cond->set_op(CondOp::Equals);
	cond->rval = new AstBool(true);
	
	//Parse the expression
	bool found_op = false;
	
	for (int i = 2; i<tokens.size()-1; i++) {
		Token t = tokens.at(i);
		
		if (tokens[i+1].type == TokenType::LEFT_PAREN) {
			std::vector<Token> sub_tokens;
			
			for (int j = i; j<tokens.size(); j++) {
				sub_tokens.push_back(tokens.at(j));
			
				if (tokens[j].type == TokenType::RIGHT_PAREN) {
					i = j;
					break;
				}
			}
			
			Line l;
			l.tokens = sub_tokens;
			
			AstFuncCall *call = build_func_call(l);
			if (found_op)
				cond->rval = call;
			else
				cond->lval = call;
			continue;
		}
		
		switch (t.type) {
			//Types
			//Variables
			case TokenType::ID: {
				AstID *id = new AstID(t.id);
				
				if (found_op)
					cond->rval = id;
				else
					cond->lval = id;
			} break;
			
			//Values
			case TokenType::NO: {
				AstInt *i = new AstInt(std::stoi(t.id));
				
				if (found_op)
					cond->rval = i;
				else
					cond->lval = i;
			} break;
			
			case TokenType::CHAR: {
				AstChar *ch = new AstChar(t.id[0]);
				
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
	}
	
	//Return
	return cond;
}

//Builds an AST node from a string of tokens
AstNode *build_node(Line ln) {
	auto tokens = ln.tokens;
	if (tokens.size() == 0)
		return nullptr;
		
	auto first = tokens.at(0);
	
	//Build an include node
	switch (first.type) {
		//Build a function declaration node
		case TokenType::EXTERN:
		case TokenType::FUNC_DEC: return build_func_dec(ln);
	
		//Build an End statement
		case TokenType::END: {
			AstNode *node = new AstNode(AstType::End);
			return node;
		}
		
		//Build a return statement
		case TokenType::RETURN: {
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
		
		//Build conditional statements
		case TokenType::IF:
		case TokenType::ELIF: return build_conditional(ln);
		case TokenType::ELSE: {
			AstElse *node = new AstElse;
			return node;
		}
		
		//Build loops
		case TokenType::WHILE: return build_conditional(ln);
		
		//Handle if the first node is an ID
		case TokenType::ID: {
			if (tokens.size() < 2) {
				syntax_error(ln, "Only an ID was specified!");
			}
			
			//Check for an array assignment
			if (tokens.size() >= 6) {
				auto t1 = tokens.at(1).type;
				auto t2 = tokens.at(3).type;
				auto t3 = tokens.at(4).type;
				
				if (t1 == TokenType::L_BRACKET && t2 == TokenType::R_BRACKET
					&& t3 == TokenType::ASSIGN) {
					Token index = tokens.at(2);
					AstArrayAssign *assign = new AstArrayAssign(first.id);
					
					switch (index.type) {
						case TokenType::ID: {
							AstID *id = new AstID(index.id);
							assign->index = id;
						} break;
						
						case TokenType::NO: {
							int val = std::stoi(index.id);
							AstInt *i = new AstInt(val);
							assign->index = i;
						} break;
						
						default: syntax_error(ln, "Invalid array access.");
					}
					
					build_var_parts(assign, 5, tokens);
					return assign;
				}
			}
			
			//Build a function call
			if (tokens.at(1).type == TokenType::LEFT_PAREN) {
				return build_func_call(ln);
				
			//Build an assignment
			} else if (tokens.at(1).type == TokenType::ASSIGN) {
				if (tokens.size() < 3) {
					syntax_error(ln, "Missing elements.");
				}
			
				AstVarAssign *va = new AstVarAssign(tokens.at(0).id);
				build_var_parts(va, 2, tokens);
				return va;
			
			//Build an increment
			} else if (tokens.at(1).type == TokenType::D_PLUS) {
				AstVarAssign *va = new AstVarAssign(tokens.at(0).id);
				AstNode *inc = new AstNode(AstType::Inc);
				va->children.push_back(inc);
				return va;
			}
		}
		
		//Build variable declarations
		//Build a byte assignment
		case TokenType::T_BYTE:
		case TokenType::T_CHAR:
		case TokenType::T_SHORT:
		case TokenType::T_INT:
		case TokenType::T_FLOAT:
		case TokenType::T_BOOL:
		case TokenType::T_STR:
		case TokenType::INT_128:
		case TokenType::INT_256:
		case TokenType::FLOAT_80:
		case TokenType::FLOAT_128:
		case TokenType::FLOAT_256: {
			if (tokens.size() > 4) {
				auto t1 = tokens[1].type;
				auto t2 = tokens[3].type;
				
				if (t1 == TokenType::L_BRACKET && t2 == TokenType::R_BRACKET) {
					AstArrayDec *arr = build_array(ln, false);
					return arr;
				}
			}
			
			auto vd = basic_var_dec(ln);
			vd->set_type(ttype2dtype(first.type));
			
			build_var_parts(vd, 3, tokens);
			
			return vd;
		}
	}

	return nullptr;
}

//Builds a tree from a given list of nodes
int build_tree(std::vector<AstNode *> nodes, AstNode *top, int index, bool in_if) {
	int i = index;
	
	for (; i<nodes.size(); i++) {
		auto c = nodes.at(i);
		
		if (c->type == AstType::FuncDec) {
			AstFuncDec *fd = dynamic_cast<AstFuncDec *>(c);
		
			AstScope *s = new AstScope;
			s->set_name(fd->get_name());
			
			top->children.push_back(c);
			c->children.push_back(s);
			
			i = build_tree(nodes, s, i+1);
		} else if (c->type == AstType::If || c->type == AstType::Elif 
				|| c->type == AstType::Else || c->type == AstType::While) {
			top->children.push_back(c);
			i = build_tree(nodes, c, i+1, true);
		} else if (c->type == AstType::End) {
			return i;
		} else {
			top->children.push_back(c);
		}
	}
	
	return i;
}
