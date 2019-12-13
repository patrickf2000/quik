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

//Builds an AST node from a string of tokens
AstNode *build_node(Line ln) {
	auto tokens = ln.tokens;
	auto first = tokens.at(0);
	
	//Build an include node
	if (first.type == TokenType::INCLUDE) {
		if (tokens.size() != 2) {
			syntax_error(ln, "Invalid size.");
		}
		
		Token id = tokens.at(1);
		if (id.type != TokenType::ID) {
			syntax_error(ln, "Second element of an include statement should be an ID.");
		}
	
		AstInclude *node = new AstInclude(id.id);
		return node;
		
	//Build a function declaration node
	//TODO: We need arguments
	} else if (first.type == TokenType::FUNC_DEC) {
		if (tokens.size() < 2) {
			syntax_error(ln, "Invalid size.");
		}
		
		Token id = tokens.at(1);
		if (id.type != TokenType::ID) {
			syntax_error(ln, "No function name specified!");
		}
		
		AstFuncDec *fd = new AstFuncDec(id.id);
		return fd;
	
	//Build an End statement
	} else if (first.type == TokenType::END) {
		AstNode *node = new AstNode(AstType::End);
		return node;
		
	//Handle if the first node is an ID
	} else if (first.type == TokenType::ID) {
		if (tokens.size() < 2) {
			syntax_error(ln, "Only an ID was specified!");
		}
		
		//Build a function call
		if (tokens.at(1).type == TokenType::LEFT_PAREN) {
			std::string name = first.id;
			std::vector<Token> args;
			Token last;
			
			for (int i = 2; i<tokens.size(); i++) {
				auto t = tokens.at(i);
				last = t;
			
				if (t.type == TokenType::RIGHT_PAREN) {
					break;
				} else if (t.type == TokenType::COMMA) {
					continue;
				}
				
				args.push_back(t);
			}
			
			if (last.type != TokenType::RIGHT_PAREN) {
				syntax_error(ln, "No closing token");
			}
			
			AstFuncCall *call = new AstFuncCall(name, args);
			return call;
			
		//Build an assignment
		} else if (tokens.at(1).type == TokenType::ASSIGN) {
			std::cout << "VAR!" << std::endl;
		}
		
	//Build variable declarations
	//Build a byte assignment
	} else if (first.type == TokenType::T_BYTE) {
	
	} else if (first.type == TokenType::T_CHAR) {
	
	} else if (first.type == TokenType::T_SHORT) {
	
	} else if (first.type == TokenType::T_INT) {
		auto vd = basic_var_dec(ln);
		vd->set_type(DataType::Int);
		
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
	} else if (first.type == TokenType::T_LONG) {
	
	} else if (first.type == TokenType::T_FLOAT) {
	
	} else if (first.type == TokenType::T_BOOL) {
	
	} else if (first.type == TokenType::T_STR) {
	
	} else if (first.type == TokenType::VAR) {
	
	}

	return nullptr;
}

//Builds a tree from a given list of nodes
int build_tree(std::vector<AstNode *> nodes, AstNode *top, int index) {
	int i = index;
	
	for (; i<nodes.size(); i++) {
		auto c = nodes.at(i);
		
		if (c->type == AstType::FuncDec) {
			top->children.push_back(c);
			i = build_tree(nodes, c, i+1);
		} else if (c->type == AstType::End) {
			return i;
		} else {
			top->children.push_back(c);
		}
	}
	
	return i;
}
