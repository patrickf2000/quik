#include "lex.hh"

//Tokenize a string
std::vector<Token> tokenize(std::string line) {
	line += " ";	//No purpose, makes my life easier :)
	
	std::vector<Token> tokens;
	std::string current = "";
	
	for (auto c : line) {
		if (c == ' ') {
			Token t;
			
			if (current == "include") {
				t.type = TokenType::INCLUDE;
			} else {
				t.type = TokenType::ID;
				t.id = current.c_str();
			}
			
			tokens.push_back(t);
			current = "";
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
	return TokenType::NONE;
}
