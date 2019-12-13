#pragma once

#include <vector>
#include <string>

enum TokenType {
	NONE,
	
	//Keywords
	INCLUDE,
	FUNC_DEC,
	END,
	
	//Type keywords
	T_BYTE,
	T_CHAR,
	T_SHORT,
	T_INT,
	T_LONG,
	T_FLOAT,
	T_BOOL,
	T_STR,
	VAR,
	
	//Symbols
	LEFT_PAREN,
	RIGHT_PAREN,
	ASSIGN,
	COMMA,
	
	//Operator
	PLUS,
	MINUS,
	MUL,
	DIV,
	MOD,
	
	//Other stuff
	ID,
	
	//Values
	STRING,
	NO,
	CHAR,
	DEC,
	B_TRUE,
	B_FALSE
};

struct Token {
	TokenType type;
	std::string id;
};

struct Line {
	std::vector<Token> tokens;
	int no;
	std::string original;
};
