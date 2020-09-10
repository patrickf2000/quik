#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stack>

#include <types.hh>

class Scanner {
public:
    TokenType getNext();
    std::vector<Line> tokenize2(std::string file);
    std::vector<Token> tokenize(std::string line);
    TokenType str2type(std::string in);
protected:
    TokenType getSymbol(char c, int i, std::string line);
    TokenType getSymbol2(char c);
private:
    std::ifstream reader;
    std::string currentLn = "";
    std::string sval = "";
    
    std::stack<TokenType> tokenStack;
    
    bool skip_next = false;
};


