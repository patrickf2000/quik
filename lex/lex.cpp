#include "lex.hh"

bool is_separator(char c) {
	if (c == ' ' || c == '(' || c == ')') {
		return true;
	}
	
	return false;
}
#include <iostream>
//Tokenize a string
std::vector<Token> tokenize(std::string line) {
	line += " ";	//No purpose, makes my life easier :)
	
	std::vector<Token> tokens;
	std::string current = "";
	
	for (auto c : line) {
		if (is_separator(c)) {
			Token t;
			
			if (current == "include") {
				t.type = TokenType::INCLUDE;
				tokens.push_back(t);
			} else if (current == "fn") {
				t.type = TokenType::FUNC_DEC;
				tokens.push_back(t);
			} else if (current.size() > 0) {
				t.type = TokenType::ID;
				t.id = current.c_str();
				tokens.push_back(t);
			}
			
			current = "";
			
			if (c == '(') {
				Token t;
				t.type = TokenType::LEFT_PAREN;
				tokens.push_back(t);
			} else if (c == ')') {
				Token t;
				t.type = TokenType::RIGHT_PAREN;
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
	return TokenType::NONE;
}
