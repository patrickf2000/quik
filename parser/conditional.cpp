#include "parse.hh"

//Builds a conditional statement
AstCond *QkParser::build_conditional(Line ln) {
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
			
			if (found_op)
				cond->rval = math;
			else
				cond->lval = math;
			continue;
		} else if (tokens[i+1].type == TokenType::LEFT_PAREN
			&& tokens[i].type == TokenType::ID) {
			std::vector<Token> sub_tokens;
            
            std::string name = tokens[i].id;
            currentIndex = i + 1;
			
			for (int j = i; j<tokens.size(); j++) {
				sub_tokens.push_back(tokens.at(j));
			
				if (tokens[j].type == TokenType::RIGHT_PAREN) {
					i = j;
					break;
				}
			}
			
			Line l;
			l.tokens = sub_tokens;
			
			AstFuncCall *call = build_func_call(name);
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
