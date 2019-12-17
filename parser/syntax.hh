#pragma once

#include <string>
#include <vector>

#include "ast.hh"

struct Error {
	std::string msg;
	Line ln;
};

class SyntaxCheck {
public:
	void check_global(AstNode *top);
	void evaluate();
private:
	std::vector<Error> errors;
};
