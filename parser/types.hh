#pragma once

#include <vector>
#include <string>

enum TokenType {
	NONE,
	
	//Keywords
	FUNC_DEC,
	EXTERN,
	END,
	IF,
	ELIF,
	ELSE,
	WHILE,
	RETURN,
	
	//Type keywords
	T_BYTE,
	T_CHAR,
	T_SHORT,
	T_INT,
	T_LONG,
	T_FLOAT,
	T_BOOL,
	T_STR,
	FLOAT_128,
	FLOAT_256,
	FLOAT_80,
	VAR,
	
	//Symbols
	LEFT_PAREN,
	RIGHT_PAREN,
	L_BRACKET,
	R_BRACKET,
	ASSIGN,
	COMMA,
	COLON,
	
	//Operator
	PLUS,
	MINUS,
	MUL,
	DIV,
	MOD,
	D_PLUS,
	
	//Boolean operators
	EQUALS,
	NOT_EQUAL,
	NOT,
	GREATER,
	LESS,
	GREATER_EQ,
	LESS_EQ,
	
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

enum class DataType {
	None,
	Byte,
	Char,
	Short,
	Int,
	Long,
	Float,
	Bool,
	Str,
	Float128,
	Float256,
	Float80
};

enum class CondOp {
	None,
	Equals,
	NotEquals,
	Greater,
	GreaterEq,
	Less,
	LessEq
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
