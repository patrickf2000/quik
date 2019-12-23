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
	void check_vars(AstNode *top, std::map<std::string, Var> vars);
	void evaluate();
private:
	std::vector<Error> errors;
};
