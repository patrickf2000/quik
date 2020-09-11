#include <types.hh>

std::string token2str(TokenType t) {
    switch (t) {
        case TokenType::NL: return "\\n";
        case TokenType::FUNC_DEC: return "func";
        case TokenType::EXTERN: return "extern";
        case TokenType::GLOBAL: return "global";
        case TokenType::END: return "end";
        case TokenType::IF: return "if";
        case TokenType::ELIF: return "elif";
        case TokenType::ELSE: return "else";
        case TokenType::WHILE: return "while";
        case TokenType::LOOP: return "loop";
        case TokenType::FOREACH: return "foreach";
        case TokenType::RETURN: return "return";
        
        case TokenType::STRUCT: return "struct";
        
        case TokenType::T_VOID: return "void";
        case TokenType::T_BOOL: return "bool";
        case TokenType::T_BYTE: return "byte";
        case TokenType::T_UBYTE: return "ubyte";
        case TokenType::T_SHORT: return "short";
        case TokenType::T_USHORT: return "ushort";
        case TokenType::T_INT: return "int";
        case TokenType::T_UINT: return "uint";
        case TokenType::T_LONG: return "long";
        case TokenType::T_ULONG: return "ulong";
        case TokenType::T_FLOAT: return "float";
        case TokenType::T_DOUBLE: return "double";
        case TokenType::T_CHAR: return "char";
        case TokenType::T_CHAR16: return "char16";
        case TokenType::T_CHAR32: return "char32";
        case TokenType::INT_64: return "int64";
        case TokenType::INT_128: return "int128";
        case TokenType::INT_256: return "int256";
        case TokenType::UINT_64: return "uint64";
        case TokenType::UINT_128: return "uint128";
        case TokenType::UINT_256: return "uint256";
        case TokenType::FLOAT_64: return "float64";
        case TokenType::FLOAT_128: return "float128";
        case TokenType::FLOAT_256: return "float256";
        case TokenType::DOUBLE_128: return "double128";
        case TokenType::DOUBLE_256: return "double256";
        case TokenType::T_STR: return "str";
        
        //Symbols
        case TokenType::LEFT_PAREN: return "(";
        case TokenType::RIGHT_PAREN: return ")";
        case TokenType::L_BRACKET: return "[";
        case TokenType::R_BRACKET: return "]";
        case TokenType::ASSIGN: return "=";
        case TokenType::COMMA: return ",";
        case TokenType::COLON: return ":";
        case TokenType::DOT: return ".";
        
        //Operator
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::MUL: return "*";
        case TokenType::DIV: return "/";
        case TokenType::MOD: return "%";
        case TokenType::D_PLUS: return "++";
        case TokenType::D_MUL: return "**";
        case TokenType::AND: return "&&";
        case TokenType::OR: return "OR";
        case TokenType::XOR: return "XOR";
        
        //Boolean operators
        case TokenType::EQUALS: return "==";
        case TokenType::NOT_EQUAL: return "!=";
        case TokenType::NOT: return "!";
        case TokenType::GREATER: return ">";
        case TokenType::LESS: return "<";
        case TokenType::GREATER_EQ: return ">=";
        case TokenType::LESS_EQ: return "<=";
        
        //Other stuff
        case TokenType::ID: return "ID";
        
        //Values
        case TokenType::HEX: return "HexL";
        case TokenType::STRING: return "StrL";
        case TokenType::NO: return "IntL";
        case TokenType::CHAR: return "CharL";
        case TokenType::DEC: return "FloatL";
        case TokenType::B_TRUE: return "True";
        case TokenType::B_FALSE: return "False";
    }

    return "NONE";
}
