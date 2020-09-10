#pragma once

#include <vector>
#include <string>

#include "types.hh"

class Scanner {
public:
    TokenType getNext(std::string line);
    std::vector<Token> tokenize(std::string line);
    TokenType str2type(std::string in);
protected:
    TokenType getSymbol(char c, int i, std::string line);
private:
    bool skip_next = false;
};


