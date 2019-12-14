#include <iostream>
#include <cstdlib>

#include "parse.hh"
#include "types.hh"

//Throws a syntax error and exits the program
void syntax_error(Line ln, std::string msg) {
	std::cout << "Syntax Error: " << msg << std::endl;
	std::cout << "[" << ln.no << "] " << ln.original << std::endl;
	std::exit(1);
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

//Builds an AST node from a string of tokens
AstNode *build_node(Line ln) {
	auto tokens = ln.tokens;
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
		//TODO: We need arguments
		case TokenType::FUNC_DEC: {
			if (tokens.size() < 2) {
				syntax_error(ln, "Invalid size.");
			}
			
			Token id = tokens.at(1);
			if (id.type != TokenType::ID) {
				syntax_error(ln, "No function name specified!");
			}
			
			AstFuncDec *fd = new AstFuncDec(id.id);
			return fd;
		}
	
		//Build an End statement
		case TokenType::END: {
			AstNode *node = new AstNode(AstType::End);
			return node;
		}
		
		//Handle if the first node is an ID
		case TokenType::ID: {
			if (tokens.size() < 2) {
				syntax_error(ln, "Only an ID was specified!");
			}
			
			//Build a function call
			if (tokens.at(1).type == TokenType::LEFT_PAREN) {
				std::string name = first.id;
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
				
			//Build an assignment
			} else if (tokens.at(1).type == TokenType::ASSIGN) {
				std::cout << "VAR!" << std::endl;
				return nullptr;
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
			
			for (int i = 3; i<tokens.size(); i++) {
				Token t = tokens.at(i);
				
				if (t.type == TokenType::NO) {
					int no = std::stoi(t.id);
					AstInt *i = new AstInt(no);
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
			
			return vd;
		}
	}

	return nullptr;
}

//Builds a tree from a given list of nodes
int build_tree(std::vector<AstNode *> nodes, AstNode *top, int index) {
	int i = index;
	
	for (; i<nodes.size(); i++) {
		auto c = nodes.at(i);
		
		if (c->type == AstType::FuncDec) {
			AstScope *s = new AstScope;
			top->children.push_back(c);
			c->children.push_back(s);
			i = build_tree(nodes, s, i+1);
		} else if (c->type == AstType::End) {
			return i;
		} else {
			top->children.push_back(c);
		}
	}
	
	return i;
}
