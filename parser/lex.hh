#pragma once

#include <vector>
#include <string>

enum TokenType {
	NONE,
	
	//Keywords
	INCLUDE,
	FUNC_DEC,
	END,
	
	//Symbols
	LEFT_PAREN,
	RIGHT_PAREN,
	ASSIGN,
	COMMA,
	
	//Other stuff
	ID,
	STRING
};

struct Token {
	TokenType type;
	const char *id;
};

std::vector<Token> tokenize(std::string line);
TokenType str2type(std::string in);

