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

struct Line {
	std::vector<Token> tokens;
	int no;
	std::string original;
};
