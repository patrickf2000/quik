#pragma once

#include <vector>
#include <string>

enum TokenType {
	NONE,
	
	//Keywords
	INCLUDE,
	FUNC_DEC,
	EXTERN,
	END,
	IF,
	ELIF,
	ELSE,
	
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
	COLON,
	
	//Operator
	PLUS,
	MINUS,
	MUL,
	DIV,
	MOD,
	
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
	Str
};

enum class CondOp {
	None,
	Equals,
	NotEquals
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
