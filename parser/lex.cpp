#include <cctype>
#include <iostream>
#include <cstdlib>

#include "lex.hh"
#include "strutils.hh"

bool is_separator(char c) {
	if (c == ' ' || c == '(' || c == ')'
		|| c == '=') {
		return true;
	}
	
	return false;
}

bool is_int(std::string s) {
	for (char c : s) {
		if (!isdigit(c))
			return false;
	}
	return true;
}

bool is_dec(std::string s) {
	for (char c : s) {
		if (!isdigit(c) && c != '.')
			return false;
	}
	
	return true;
}

//Tokenize a string
std::vector<Token> tokenize(std::string line) {
	line += " ";	//No purpose, makes my life easier :)
	
	std::vector<Token> tokens;
	std::string current = "";
	bool in_quote = false;
	
	for (auto c : line) {
		//First, check against double quotes
		if ((c == '\"' || c == '\'') && in_quote) {
			Token t;
			t.type = TokenType::STRING;
			t.id = current.c_str();
			
			if (c == '\'') {
				t.type = TokenType::CHAR;
				if (current.length() > 1) {
					std::cout << "Lexical Error: A char literal must be of length 1." << std::endl;
					std::cout << "> " << line << std::endl;
					std::exit(1);
				}
			}
			
			tokens.push_back(t);
			
			current = "";
			in_quote = false;
			continue;
		} else if (c == '\"' || c == '\'') {
			in_quote = true;
			continue;
		} else if (in_quote) {
			current += c;
			continue;
			
		//Check to see if we have a separator
		//If we do, that signifies we have at least one
		// new token
		} else if (is_separator(c)) {
			Token t;
			
			//Check for keywords
			if (current == "include") {
				t.type = TokenType::INCLUDE;
				tokens.push_back(t);
				
			//Function keywords
			} else if (current == "fn") {
				t.type = TokenType::FUNC_DEC;
				tokens.push_back(t);
			} else if (current == "end") {
				t.type = TokenType::END;
				tokens.push_back(t);
				
			//Variable types
			} else if (current == "byte") {
				t.type = TokenType::T_BYTE;
				tokens.push_back(t);
			} else if (current == "char") {
				t.type = TokenType::T_CHAR;
				tokens.push_back(t);
			} else if (current == "short") {
				t.type = TokenType::T_SHORT;
				tokens.push_back(t);
			} else if (current == "int") {
				t.type = TokenType::T_INT;
				tokens.push_back(t);
			} else if (current == "long") {
				t.type = TokenType::T_LONG;
				tokens.push_back(t);
			} else if (current == "float") {
				t.type = TokenType::T_FLOAT;
				tokens.push_back(t);
			} else if (current == "bool") {
				t.type = TokenType::T_BOOL;
				tokens.push_back(t);
			} else if (current == "str") {
				t.type = TokenType::T_STR;
				tokens.push_back(t);
			} else if (current == "var") {
				t.type = TokenType::VAR;
				tokens.push_back(t);
				
			//Booleans
			} else if (current == "true") {
				t.type = TokenType::B_TRUE;
				tokens.push_back(t);
			} else if (current == "false") {
				t.type = TokenType::B_FALSE;
				tokens.push_back(t);
				
			//If no keyword, and its not empty, then we have an identifier
			} else if (current.size() > 0) {
				if (is_int(current)) {
					t.type = TokenType::NO;
				} else if (is_dec(current)) {
					t.type = TokenType::DEC;
				} else {
					t.type = TokenType::ID;
				}
				
				t.id = current;
				tokens.push_back(t);
			}
			
			current = "";
			
			//Check to see if the separator is also a token
			if (c == '(') {
				Token t;
				t.type = TokenType::LEFT_PAREN;
				tokens.push_back(t);
			} else if (c == ')') {
				Token t;
				t.type = TokenType::RIGHT_PAREN;
				tokens.push_back(t);
			} else if (c == '=') {
				Token t;
				t.type = TokenType::ASSIGN;
				tokens.push_back(t);
			}
		} else {
			current += c;
		}
	}
	
	return tokens;
}

//Convert a string to a token
TokenType str2type(std::string in) {
	if (in == "INCLUDE") return TokenType::INCLUDE;
	else if (in == "ID") return TokenType::ID;
	else if (in == "FUNC_DEC") return TokenType::FUNC_DEC;
	else if (in == "LEFT_PAREN") return TokenType::LEFT_PAREN;
	else if (in == "RIGHT_PAREN") return TokenType::RIGHT_PAREN;
	else if (in == "STRING") return TokenType::STRING;
	else if (in == "END") return TokenType::END;
	else if (in == "COMMA") return TokenType::COMMA;
	else if (in == "ASSIGN") return TokenType::ASSIGN;
	else if (in == "T_BYTE") return TokenType::T_BYTE;
	else if (in == "T_CHAR") return TokenType::T_CHAR;
	else if (in == "T_SHORT") return TokenType::T_SHORT;
	else if (in == "T_INT") return TokenType::T_INT;
	else if (in == "T_LONG") return TokenType::T_LONG;
	else if (in == "T_FLOAT") return TokenType::T_FLOAT;
	else if (in == "T_BOOL") return TokenType::T_BOOL;
	else if (in == "T_STR") return TokenType::T_STR;
	else if (in == "VAR") return TokenType::VAR;
	else if (in == "NO") return TokenType::NO;
	else if (in == "CHAR") return TokenType::CHAR;
	else if (in == "DEC") return TokenType::DEC;
	else if (in == "B_TRUE") return TokenType::B_TRUE;
	else if (in == "B_FALSE") return TokenType::B_FALSE;
	return TokenType::NONE;
}
