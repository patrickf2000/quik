#include "lex.hh"

//Tokenize a string
std::vector<Token> tokenize(std::string line) {
	std::vector<Token> tokens;
	
	return tokens;
}

//Convert a string to a token
TokenType str2type(std::string in) {
	if (in == "INCLUDE") return TokenType::INCLUDE;
	else if (in == "ID") return TokenType::ID;
	return TokenType::NONE;
}
