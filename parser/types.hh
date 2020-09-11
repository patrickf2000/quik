#pragma once

#include <vector>
#include <string>

enum TokenType {
    NONE,
    NL,
    
    //Keywords
    FUNC_DEC,
    EXTERN,
    GLOBAL,
    END,
    IF,
    ELIF,
    ELSE,
    WHILE,
    LOOP,
    FOREACH,
    RETURN,
    
    //Struct keywords
    STRUCT,
    
    //Type keywords
    T_VOID,
    T_BOOL,
    T_BYTE,
    T_UBYTE,
    T_SHORT,
    T_USHORT,
    T_INT,
    T_UINT,
    T_LONG,
    T_ULONG,
    T_FLOAT,
    T_DOUBLE,
    T_CHAR,
    T_CHAR16,
    T_CHAR32,
    INT_64,
    INT_128,
    INT_256,
    UINT_64,
    UINT_128,
    UINT_256,
    FLOAT_64,
    FLOAT_128,
    FLOAT_256,
    DOUBLE_128,
    DOUBLE_256,
    T_MATRIX,
    T_STR,
    
    //Symbols
    LEFT_PAREN,
    RIGHT_PAREN,
    L_BRACKET,
    R_BRACKET,
    ASSIGN,
    COMMA,
    COLON,
    DOT,
    
    //Operator
    PLUS,
    MINUS,
    MUL,
    DIV,
    MOD,
    D_PLUS,
    D_MUL,
    AND,
    OR,
    XOR,
    
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
    HEX,
    STRING,
    NO,
    CHAR,
    DEC,
    B_TRUE,
    B_FALSE
};

enum class DataType {
    None,
    Void,
    Bool,
    Byte,
    UByte,
    Short,
    UShort,
    Int,
    UInt,
    Long,
    ULong,
    Float,
    Double,
    Char,
    Char16,
    Char32,
    Int64,
    Int128,
    Int256,
    UInt64,
    UInt128,
    UInt256,
    Float64,
    Float128,
    Float256,
    Double128,
    Double256,
    Matrix,
    Str
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

// Temporary
std::string token2str(TokenType t);
