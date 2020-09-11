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

void syntax_error(int no, std::string ln, std::string msg, bool exit) {
    Error e;
	e.ln.no = no;
    e.ln.original = ln;
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
		
		case TokenType::INT_64: return DataType::Int64;
		case TokenType::INT_128: return DataType::Int128;
		case TokenType::INT_256: return DataType::Int256;
		
		case TokenType::FLOAT_64: return DataType::Float64;
		case TokenType::FLOAT_128: return DataType::Float128;
		case TokenType::FLOAT_256: return DataType::Float256;
	}
	
	return DataType::None;
}

//Builds an AST node from a string of tokens
AstNode *QkParser::build_node(Line ln) {
    // For the temporary parser functions
    currentLn = ln;
    currentIndex = 1;
    
	auto tokens = ln.tokens;
	if (tokens.size() == 0)
		return nullptr;
		
	auto first = tokens.at(0);
	
	//Build an include node
	switch (first.type) {
		//Build a function declaration node
		case TokenType::EXTERN:
		case TokenType::GLOBAL:
		case TokenType::FUNC_DEC: return build_func_dec(ln);
	
		//Build an End statement
		case TokenType::END: {
			AstNode *node = new AstNode(AstType::End);
			return node;
		}
		
		//Build a return statement
		case TokenType::RETURN: return build_ret();
		
		//Build structure statements
		case TokenType::STRUCT: {
			if (ln.tokens.size() == 2)
				return build_struct_dec(ln);
			else
				return build_struct_var(ln);
		}
		
		//Build conditional statements
		case TokenType::IF: return build_conditional(TokenType::IF);
		case TokenType::ELIF: return build_conditional(TokenType::ELIF);
		case TokenType::ELSE: {
			AstElse *node = new AstElse;
			return node;
		}
		
		//Build loops
		case TokenType::WHILE: return build_conditional(TokenType::WHILE);
		case TokenType::LOOP: return build_loop();
		case TokenType::FOREACH: return build_foreach();
		
		//Handle if the first node is an ID
		case TokenType::ID: return build_id(getSVal());
		
		//Build variable declarations
		//Build a byte assignment
		case TokenType::T_BYTE:
		case TokenType::T_CHAR:
		case TokenType::T_SHORT:
		case TokenType::T_INT:
		case TokenType::T_FLOAT:
		case TokenType::T_BOOL:
		case TokenType::T_STR:
		case TokenType::INT_64:
		case TokenType::INT_128:
		case TokenType::INT_256:
		case TokenType::FLOAT_64:
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

//Builds a node based on an ID value
AstNode *QkParser::build_id(std::string name) {
	auto token = getNext();

	if (token == TokenType::NL) {
		syntax_error(getLnNo(), getCurrentLn(), 
            "Invalid assignment.");
	}

	//Check for an array assignment
	if (token == TokenType::L_BRACKET) {
        AstArrayAssign *assign = new AstArrayAssign(name);
        token = getNext();
        
        switch (token) {
            case TokenType::ID: {
                auto val = getSVal();
                AstID *id = new AstID(val);
                assign->index = id;
            } break;
				
            case TokenType::NO: {
                int val = getIVal();
                AstInt *i = new AstInt(val);
                assign->index = i;
            } break;
				
            default: syntax_error(getLnNo(), getCurrentLn(), 
                        "Invalid array access.");
        }
        
        token = getNext();
        auto next = getNext();
        
        if (token != TokenType::R_BRACKET || next != TokenType::ASSIGN) {
            syntax_error(getLnNo(), getCurrentLn(),
                "Invalid array access.");
        }
        
        //TODO: Replace this
        token = getNext();
        std::vector<Token> sub_tokens;
        
        while (token != TokenType::NL && token != TokenType::NONE) {
            Token t;
            t.type = token;
            t.id = getSVal();
            sub_tokens.push_back(t);
            
            token = getNext();
        }
        
        build_var_parts(assign, 0, sub_tokens);
        return assign;
	}

	//Build a function call
	if (token == TokenType::LEFT_PAREN) {
        //getNext();
		return build_func_call(name);
		
	//Build an assignment
	} else if (token == TokenType::ASSIGN) {
        token = getNext();
		if (token == TokenType::NL) {
			syntax_error(getLnNo(), getCurrentLn(),
                "Missing elements.");
		}
        
        // TODO: Replace this loop
        std::vector<Token> sub_tokens;
        
        while (token != TokenType::NL && token != TokenType::NONE) {
            Token t;
            t.type = token;
            t.id = getSVal();
            sub_tokens.push_back(t);
            
            token = getNext();
        }

        // Build the assignment
		AstVarAssign *va = new AstVarAssign(name);
		build_var_parts(va, 0, sub_tokens);
		return va;
		
	//Build a multiple variable assignment
	} else if (token == TokenType::COMMA) {
		auto mva = new AstMultiVarAssign;
        
        auto var1 = new AstID(name);
        mva->vars.push_back(var1);
        
        auto token = getNext();
        
        while (token != TokenType::ASSIGN && token != TokenType::NONE) {
            if (token == TokenType::COMMA) {
                token = getNext();
                continue;
            } else if (token == TokenType::ID) {
                auto val = getSVal();
                auto var = new AstID(val);
                mva->vars.push_back(var);
            } else {
                syntax_error(getLnNo(), getCurrentLn(),
                    "Invalid multi-variable assignment.");
            }
            
            token = getNext();
        }
        
        // TODO: Replace this
        token = getNext();
        std::vector<Token> sub_tokens;
        
        while (token != TokenType::NL && token != TokenType::NONE) {
            Token t;
            t.type = token;
            t.id = getSVal();
            sub_tokens.push_back(t);
            
            token = getNext();
        }
		
		build_var_parts(mva, 0, sub_tokens);
		return mva;
		
	//Build a structure assignment
	} else if (token == TokenType::DOT) {
		/*if (tokens.size() < 5)
			syntax_error(ln, "Missing elements.");
		
		if (tokens.at(3).type != TokenType::ASSIGN)
			syntax_error(ln, "Expected \'=\'");

		Token s_var = tokens.at(0);
		Token i_var = tokens.at(2);
		
		if (s_var.type != TokenType::ID)
			syntax_error(ln, "Invalid structure assignment.");
			
		if (i_var.type != TokenType::ID)
			syntax_error(ln, "Invalid structure member assignment.");
		
		AstStructMod *mod = new AstStructMod;
		mod->str_name = s_var.id;
		mod->var_name = i_var.id;
		
		build_var_parts(mod, 4, tokens);
		return mod;*/

	//Build an increment
	} else if (token == TokenType::D_PLUS) {
		AstVarAssign *va = new AstVarAssign(name);
		AstNode *inc = new AstNode(AstType::Inc);
		va->children.push_back(inc);
		return va;
	}
	
	//TODO: Throw a syntax error, don't blow the thing up
	return nullptr;
}

//Builds a tree from a given list of nodes
int QkParser::build_tree(std::vector<AstNode *> nodes, AstNode *top, int index, bool in_if) {
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
				|| c->type == AstType::Else || c->type == AstType::While
				|| c->type == AstType::Loop || c->type == AstType::ForEach
				|| c->type == AstType::StructDec) {
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

//A wrapper function for calling all the analzyer and check functions
void QkParser::analyze_and_check(AstScope *top) {
	find_variables(top);
	find_assign(top, top);
	find_cond(top);
	check_return(top);
}

