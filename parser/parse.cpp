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
		
		case TokenType::INT_64: return DataType::Int64;
		case TokenType::INT_128: return DataType::Int128;
		case TokenType::INT_256: return DataType::Int256;
		
		case TokenType::FLOAT_64: return DataType::Float64;
		case TokenType::FLOAT_80: return DataType::Float80;
		case TokenType::FLOAT_128: return DataType::Float128;
		case TokenType::FLOAT_256: return DataType::Float256;
	}
	
	return DataType::None;
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

//Builds a loop statement
AstLoop *build_loop(Line ln) {
	auto tokens = ln.tokens;
	if (tokens.size() > 2)
		syntax_error(ln, "Too many arguments in loop statement.");

	AstLoop *lp = new AstLoop;
	lp->param = nullptr;
	
	if (tokens.size() > 1) {
		auto arg = tokens.at(1);
		
		switch (arg.type) {
			case TokenType::NO: {
				int i = std::stoi(arg.id);
				lp->param = new AstInt(i);
			} break;
			
			case TokenType::ID: {
				AstID *id = new AstID(arg.id);
				lp->param = id;
			} break;
			
			default: {
				std::string er_msg = "Error: Only integers and integer variables ";
				er_msg += "may be used with the loop statement.";
				syntax_error(ln, er_msg);
			}
		}
	}
	
	return lp;
}

//Builds a foreach statement
AstForEach *build_foreach(Line ln) {
	auto tokens = ln.tokens;
	if (tokens.size() != 4)
		syntax_error(ln, "Insufficient or too many arguments.");
		
	if (tokens.at(2).type != TokenType::COLON)
		syntax_error(ln, "Invalid foreach syntax.");

	AstForEach *fe = new AstForEach;
	auto i_var = tokens.at(1);
	auto r_var = tokens.at(3);
	
	if (i_var.type != TokenType::ID)
		syntax_error(ln, "Iterator must be a variable.");
		
	if (r_var.type != TokenType::ID)
		syntax_error(ln, "Range must be an array or vector variable.");
		
	fe->i_var = i_var.id;
	fe->r_var = r_var.id;
	
	return fe;
}

//Builds a structure declaration
AstStructDec *build_struct_dec(Line ln) {
	AstStructDec *dec = new AstStructDec;
	
	Token name = ln.tokens.at(1);
	if (name.type != TokenType::ID)
		syntax_error(ln, "Invalid structure identifier.");
		
	dec->set_name(name.id);
	
	return dec;
}

//Builds a structure operation
AstStruct *build_struct_var(Line ln) {
	AstStruct *op = new AstStruct;
	auto tokens = ln.tokens;
	
	//Perform a size check
	if (tokens.size() != 3)
		syntax_error(ln, "Invalid tokens in structure declaration.");
		
	auto str_name = tokens.at(1);
	auto var_name = tokens.at(2);
	
	//Perform some syntax checking
	if (str_name.type != TokenType::ID)
		syntax_error(ln, "Invalid or non-existent structure being referenced.");
		
	if (var_name.type != TokenType::ID)
		syntax_error(ln, "Invalid structure variable name.");
		
	//Create the AST type
	op->str_name = str_name.id;
	op->var_name = var_name.id;
	
	return op;
}

//Builds an AST node from a string of tokens
AstNode *QkParser::build_node(Line ln) {
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
		case TokenType::RETURN: return build_ret(ln);
		
		//Build structure statements
		case TokenType::STRUCT: {
			if (ln.tokens.size() == 2)
				return build_struct_dec(ln);
			else
				return build_struct_var(ln);
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
		case TokenType::LOOP: return build_loop(ln);
		case TokenType::FOREACH: return build_foreach(ln);
		
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
				
			//Build a structure assignment
			} else if (tokens.at(1).type == TokenType::DOT) {
				if (tokens.size() < 5)
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
				return mod;
			
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
		case TokenType::INT_64:
		case TokenType::INT_128:
		case TokenType::INT_256:
		case TokenType::FLOAT_64:
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
