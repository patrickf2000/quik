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
    TokenType getSymbol(char c);
private:
    std::ifstream reader;
    std::string currentLn = "";
    std::string sval = "";
    
    std::stack<TokenType> tokenStack;
};


