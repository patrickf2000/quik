#pragma once

#include <vector>
#include <lex.hh>

#include "ast.hh"

void syntax_error(Line ln, std::string msg, bool exit = true);
AstNode *build_node(Line ln);
int build_tree(std::vector<AstNode *> nodes, AstNode *top, int index = 0, bool in_if = false);
