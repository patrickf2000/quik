#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stack>

#include <types.hh>

class Scanner {
public:
    TokenType getNext();
    std::vector<Line> tokenize(std::string file);
protected:
    bool isSeparator(char c);
    bool isInt(std::string s);
    bool isFloatL(std::string s);
    bool isHex(std::string s);
    TokenType getSymbol(char c);
    TokenType getKeyword(std::string current);
private:
    std::ifstream reader;
    std::string currentLn = "";
    std::string sval = "";
    
    std::stack<TokenType> tokenStack;
};


