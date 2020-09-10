#include <cctype>
#include <iostream>
#include <cstdlib>

#include "lex.hh"
#include "strutils.hh"

bool is_separator(char c) {
	if (c == ' ' || c == '\n' || c == '(' || c == ')'
		|| c == '[' || c == ']' || c == '&' || c == '?'
		|| c == '=' || c == ',' || c == ':' || c == '.'
		|| c == '+' || c == '-' || c == '*' || c == '/'
		|| c == '%' || c == '!' || c == '>' || c == '<') {
		return true;
	}
	
	return false;
}

bool is_int(std::string s) {
    if (s.length() == 0)
        return false;
    
	int index = 0;
	
	for (char c : s) {
		if (c == '-') {
			if (index > 0) {
				return false;
			}
			
			++index;
			continue;
		}
		
		if (!isdigit(c))
			return false;
			
		++index;
	}
	return true;
}

bool is_dec(std::string s) {
	int index = 0;

	for (char c : s) {
		if (c == '-') {
			if (index > 0) {
				return false;
			}
			
			++index;
			continue;
		}
	
		if (!isdigit(c) && c != '.')
			return false;
			
		++index;
	}
	
	return true;
}

bool is_hex(std::string s) {
	if (s.length() < 3)
		return false;
		
	if (s[0] != '0' || s[1] != 'x') {
		return false;
	}
	
	for (int i = 2; i<s.length(); i++) {
		char c = s[i];
		
		if (!isxdigit(c))
			return false;
	}
	
	return true;
}

bool is_last_op(std::vector<Token> tokens) {
	auto t = tokens.at(tokens.size()-1);
	
	switch (t.type) {
		case TokenType::ASSIGN:
		case TokenType::PLUS:
		case TokenType::MINUS:
		case TokenType::MUL:
		case TokenType::DIV:
		case TokenType::MOD:
		case TokenType::D_PLUS:
		case TokenType::D_MUL:
		case TokenType::AND:
		case TokenType::OR:
		case TokenType::XOR: return true;
	}
	
	return false;
}

//Check for keywords
TokenType getKeyword(std::string current) {

    //Function keywords
    if (current == "func") return TokenType::FUNC_DEC;
    else if (current == "extern") return TokenType::EXTERN;
    else if (current == "global") return TokenType::GLOBAL;
    else if (current == "end") return TokenType::END;
    else if (current == "return") return TokenType::RETURN;
    				
    //Structure
    else if (current == "struct") return TokenType::STRUCT;
    		
    //Variable types
    else if (current == "byte") return TokenType::T_BYTE;
    else if (current == "char") return TokenType::T_CHAR;
    else if (current == "short") return TokenType::T_SHORT;
    else if (current == "bool") return TokenType::T_BOOL;
    else if (current == "str") return TokenType::T_STR;
    	
    //Integer types
    else if (current == "int") return TokenType::T_INT;
    else if (current == "int64") return TokenType::INT_64;
    else if (current == "int128") return TokenType::INT_128;
    else if (current == "int256") return TokenType::INT_256;
    
    //Floating-point types
    else if (current == "float") return TokenType::T_FLOAT;
    else if (current == "float64") return TokenType::FLOAT_64;
    else if (current == "float128") return TokenType::FLOAT_128;
    else if (current == "float256") return TokenType::FLOAT_256;
    
    //Double types
    else if (current == "double") return TokenType::T_DOUBLE;
    else if (current == "double128") return TokenType::DOUBLE_128;
    else if (current == "double256") return TokenType::DOUBLE_256;
				
    //Unsigned integers
    else if (current == "uint") return TokenType::T_UINT;
    else if (current == "uint64") return TokenType::UINT_64;
    else if (current == "uint128") return TokenType::UINT_128;
    else if (current == "uint256") return TokenType::UINT_256;
    	
    //Conditional stuff
    else if (current == "if") return TokenType::IF;
    else if (current == "elif") return TokenType::ELIF;
    else if (current == "else") return TokenType::ELSE;
    	
    //Loops
    else if (current == "while") return TokenType::WHILE;
    else if (current == "loop") return TokenType::LOOP;
    else if (current == "foreach") return TokenType::FOREACH;
    
    //Booleans
    else if (current == "true") return TokenType::B_TRUE;
    else if (current == "false") return TokenType::B_FALSE;
    
    // Constants/literals
    else if (is_int(current)) return TokenType::NO;
    else if (is_dec(current)) return TokenType::DEC;
    else if (is_hex(current)) return TokenType::HEX;    
    else return TokenType::ID;
}

//Check to see if the separator is also a token
TokenType Scanner::getSymbol(char c) {
    char nc = 0;
    reader.get(nc);
        
    switch (c) {
        case '\n': reader.putback(nc); return TokenType::NL;
        case '.': reader.putback(nc); return TokenType::DOT;
        case '(': reader.putback(nc); return TokenType::LEFT_PAREN;
        case ')': reader.putback(nc); return TokenType::RIGHT_PAREN;
        case '[': reader.putback(nc); return TokenType::L_BRACKET;
        case ']': reader.putback(nc); return TokenType::R_BRACKET;
        case ',': reader.putback(nc); return TokenType::COMMA;
        case ':': reader.putback(nc); return TokenType::COLON;
        case '-': reader.putback(nc); return TokenType::MINUS;
        case '/': reader.putback(nc); return TokenType::DIV;
        case '%': reader.putback(nc); return TokenType::MOD;
        
        case '=': {
            if (nc == '=')
                return TokenType::EQUALS;
            
            reader.putback(nc);
            return TokenType::ASSIGN;
        } break;
        
        case '!': {
            if (nc == '=')
                return TokenType::NOT_EQUAL;
            
            reader.putback(nc);
            return TokenType::NOT;
        } break;
        
        case '>': {
            if (nc == '=')
                return TokenType::GREATER_EQ;
            
            reader.putback(nc);
            return TokenType::GREATER;
        } break;
        
        case '<': {
            if (nc == '=')
                return TokenType::LESS_EQ;
            
            reader.putback(nc);
            return TokenType::LESS;
        } break;
        
        case '+': {
            if (nc == '+')
                return TokenType::D_PLUS;
            
            reader.putback(nc);
            return TokenType::PLUS;
        } break;
        
        case '*': {
            if (nc == '*')
                return TokenType::D_MUL;
            
            reader.putback(nc);
            return TokenType::MUL;
        } break;
        
        case '&': {
            if (nc == '&')
                return TokenType::AND;
            
            reader.putback(nc);
        } break;
        
        case '?': {
            if (nc == '?')
                return TokenType::XOR;
            
            reader.putback(nc);
            return TokenType::OR;
        } break;
    }

    reader.putback(nc);
    return TokenType::NONE;
}

TokenType Scanner::getNext() {
    if (tokenStack.size() > 0) {
        auto t = tokenStack.top();
        tokenStack.pop();
        return t;
    }

    char c = 0;
    std::string current = "";
    bool inQuote = false;
    
    while (!reader.eof()) {
        reader.get(c);
        currentLn += c;
        
        if (c == '\"' || c == '\'') {
            if (inQuote) {
                sval = current;
                inQuote = false;
                
                if (c == '\'')
                    return TokenType::CHAR;
                    
                return TokenType::STRING;
            } else {
                inQuote = true;
                continue;
            }
        }
        
        if (inQuote) {
            current += c;
            continue;
        }
        
        if (c == '.' && is_int(current)) {
            current += c;
            continue;
        }
        
        if (c == ' ' || c == '\t') {
            if (current.length() > 0) {
                sval = current;
                return getKeyword(current);
            }
        } else if (c == '\n') {
            if (current.length() > 0) {
                tokenStack.push(TokenType::NL);
                
                sval = current;
                return getKeyword(current);
            }
            
            return TokenType::NL;
        } else if (is_separator(c)) {
            TokenType token = getSymbol(c);
        
            if (current.length() > 0) {
                if (token != TokenType::NONE)
                    tokenStack.push(token);
                    
                sval = current;
                token = getKeyword(current);
            }
            
            return token;
        } else {
            current += c;
        }
    }

    return TokenType::NONE;
}

std::vector<Line> Scanner::tokenize(std::string file) {
    std::vector<Line> lines;
    reader = std::ifstream(file);
    
    if (!reader.is_open()) {
        std::cout << "Error: Unable to open input file." << std::endl;
        std::exit(1);
    }
    
    TokenType token = getNext();
    Line ln;
    Token t;
    
    while (token != TokenType::NONE) {
        switch (token) {
            case TokenType::NL: {
                ln.original = currentLn;
                ++ln.no;
                currentLn = "";
                
                lines.push_back(ln);
                ln.tokens.clear();
                
                token = getNext();
                continue;
             } break;
            
            case TokenType::NO:
            case TokenType::DEC:
            case TokenType::HEX:
            case TokenType::ID:
            case TokenType::STRING:
            case TokenType::CHAR: t.id = sval; break;
        }
        
        t.type = token;
        ln.tokens.push_back(t);
        token = getNext();
    }
    
    return lines;
}
