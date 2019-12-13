#pragma once

#include <vector>
#include <string>

#include "types.hh"

std::vector<Token> tokenize(std::string line);
TokenType str2type(std::string in);

