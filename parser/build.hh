#pragma once

#include <string>
#include <vector>

#include "lex.hh"
#include "ast.hh"

struct Line {
	std::vector<Token> tokens;
	int no;
	std::string original;
};

std::vector<Line> load_source(const char *path);
AstNode *build_ast(std::vector<Line> lines);
