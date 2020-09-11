#include <cctype>
#include <iostream>
#include <cstdlib>

#include "lex.hh"
#include "strutils.hh"

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
        
        // Check the quotes
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
        
        // The '.' can serve multiple purposes. If everything before it
        // is an integer, however, its probably a decimal
        if (c == '.' && isInt(current)) {
            current += c;
            continue;
        }
        
        // Check whitespace
        if (c == ' ' || c == '\t') {
            if (current.length() > 0) {
                sval = current;
                return getKeyword(current);
            }
        } else if (isSeparator(c)) {
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

// TODO: Is there a way we can clean this up a little bit?
//
// The whole pre-file thing is only applicable for files run through the pre-processor
// Files run through the pre-processor will have this format:
// <no>| <the code>
//
// The number is an integer corresponding to the original line in the source file
std::vector<Line> Scanner::tokenize(std::string file, bool preFile) {
    std::vector<Line> lines;
    reader = std::ifstream(file);
    
    if (!reader.is_open()) {
        std::cout << "Error: Unable to open input file." << std::endl;
        std::exit(1);
    }
    
    Line ln;
    Token t;
    
    char c = 0;
    std::string numBuf = "";
    
    if (preFile) {
        reader.get(c);
        
        while (!reader.eof() && c != '|') {
            numBuf += c;
            reader.get(c);
        }
        
        if (isInt(numBuf)) {
            ln.no = std::stoi(numBuf);
            numBuf = "";
            c = 0;
        }
    }
    
    TokenType token = getNext();
    
    while (token != TokenType::NONE) {
        switch (token) {
            case TokenType::NL: {
                ln.original = currentLn;
                currentLn = "";
                
                lines.push_back(ln);
                ln.tokens.clear();
                
                if (preFile) {
                    reader.get(c);
                    
                    while (!reader.eof() && c != '|') {
                        numBuf += c;
                        reader.get(c);
                    }
                    
                    if (isInt(numBuf)) {
                        ln.no = std::stoi(numBuf);
                        numBuf = "";
                        c = 0;
                    }
                }
                
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

bool Scanner::isSeparator(char c) {
	if (c == ' ' || c == '\n' || c == '(' || c == ')'
		|| c == '[' || c == ']' || c == '&' || c == '?'
		|| c == '=' || c == ',' || c == ':' || c == '.'
		|| c == '+' || c == '-' || c == '*' || c == '/'
		|| c == '%' || c == '!' || c == '>' || c == '<') {
		return true;
	}
	
	return false;
}

bool Scanner::isInt(std::string s) {
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

bool Scanner::isFloatL(std::string s) {
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

bool Scanner::isHex(std::string s) {
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

//Check to see if the separator is also a token
TokenType Scanner::getSymbol(char c) {  
    switch (c) {
        case '\n': return TokenType::NL;
        case '.': return TokenType::DOT;
        case '(': return TokenType::LEFT_PAREN;
        case ')': return TokenType::RIGHT_PAREN;
        case '[': return TokenType::L_BRACKET;
        case ']': return TokenType::R_BRACKET;
        case ',': return TokenType::COMMA;
        case ':': return TokenType::COLON;
        case '-': return TokenType::MINUS;
        case '/': return TokenType::DIV;
        case '%': return TokenType::MOD;
    }
    
    // Check multi-character symbols
    // This is in a separate so we can get and possbily put back
    // the first character.
    char nc = 0;
    reader.get(nc);
    
    switch (c) {    
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

//Check for keywords
TokenType Scanner::getKeyword(std::string current) {

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
    else if (isInt(current)) return TokenType::NO;
    else if (isFloatL(current)) return TokenType::DEC;
    else if (isHex(current)) return TokenType::HEX;    
    else return TokenType::ID;
}
