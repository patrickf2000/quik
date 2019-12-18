#include <iostream>
#include <cstdlib>

#include "parse.hh"
#include "types.hh"

//Throws a syntax error and exits the program
void syntax_error(Line ln, std::string msg, bool exit) {
	std::cout << "Syntax Error: " << msg << std::endl;
	std::cout << "[" << ln.no << "] " << ln.original << std::endl;
	
	if (exit)
		std::exit(1);
}

//Translates a token type to a datatype
DataType ttype2dtype(TokenType t) {
	switch (t) {
		case TokenType::T_BYTE: return DataType::Byte;
		case TokenType::T_CHAR: return DataType::Char;
		case TokenType::T_SHORT: return DataType::Short;
		case TokenType::T_INT: return DataType::Int;
		case TokenType::T_LONG: return DataType::Long;
		case TokenType::T_FLOAT: return DataType::Float;
		case TokenType::T_BOOL: return DataType::Bool;
		case TokenType::T_STR: return DataType::Str;
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
		} else if (t.type == TokenType::DEC) {
			AstFloat *v_flt = new AstFloat(std::stod(t.id));
			call->children.push_back(v_flt);
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
void build_var_parts(AstVarDec *vd, int start, std::vector<Token> tokens) {
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
		} else {
			if (t.type == TokenType::NO) {
				int no = std::stoi(t.id);
				AstInt *i = new AstInt(no);
				vd->children.push_back(i);
			} else if (t.type == TokenType::DEC) {
				double no = std::stod(t.id);
				AstFloat *i = new AstFloat(no);
				vd->children.push_back(i);
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
				if (v.type == DataType::None) {
					syntax_error(ln, "No datatype specified in parameter");
				}
				
				fd->args.push_back(v);
			} else if (t.type == TokenType::ID) {
				v.name = id.id + "_" + t.id;
			} else if (last == TokenType::COLON) {
				v.type = ttype2dtype(t.type);
			}
				
			last = t.type;
		}
	}

	return fd;
}

//Builds a conditional statement
AstCond *build_conditional(Line ln) {
	auto tokens = ln.tokens;
	auto type = tokens.at(0).type;
	
	//Make sure the syntax is correct
	if (tokens.size() != 6) {
		syntax_error(ln, "Incorrect conditional syntax.");
	}
	
	auto t1 = tokens.at(1).type;
	auto t2 = tokens.at(5).type;
	
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
	
	//Extract the needed tokens
	auto lval = tokens.at(2);
	auto op = tokens.at(3).type;
	auto rval = tokens.at(4);
	
	//TODO: Is there a way we can clean this up?
	//Parse the left value
	switch (lval.type) {
		case TokenType::ID: {
			AstID *id = new AstID(lval.id);
			cond->lval = id;
		} break;
		case TokenType::NO: {
			AstInt *i = new AstInt(std::stoi(lval.id));
			cond->lval = i;
		} break;
	}
	
	//Right value
	switch (rval.type) {
		case TokenType::ID: {
			AstID *id = new AstID(rval.id);
			cond->rval = id;
		} break;
		case TokenType::NO: {
			AstInt *i = new AstInt(std::stoi(rval.id));
			cond->rval = i;
		} break;
	}
	
	//The operator
	switch (op) {
		case TokenType::EQUALS: cond->set_op(CondOp::Equals); break;
		case TokenType::NOT_EQUAL: cond->set_op(CondOp::NotEquals); break;
		case TokenType::GREATER: cond->set_op(CondOp::Greater); break;
		case TokenType::GREATER_EQ: cond->set_op(CondOp::GreaterEq); break;
		case TokenType::LESS: cond->set_op(CondOp::Less); break;
		case TokenType::LESS_EQ: cond->set_op(CondOp::LessEq); break;
	}
	
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
		case TokenType::INCLUDE: {
			if (tokens.size() != 2) {
				syntax_error(ln, "Invalid size.");
			}
			
			Token id = tokens.at(1);
			if (id.type != TokenType::ID) {
				syntax_error(ln, "Second element of an include statement should be an ID.");
			}
		
			AstInclude *node = new AstInclude(id.id);
			return node;
		}
		
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
			}
		}
		
		//Build variable declarations
		//Build a byte assignment
		case TokenType::T_BYTE:
		case TokenType::T_CHAR:
		case TokenType::T_SHORT:
		case TokenType::T_INT:
		case TokenType::T_LONG:
		case TokenType::T_FLOAT:
		case TokenType::T_BOOL:
		case TokenType::T_STR:
		case TokenType::VAR: {
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
