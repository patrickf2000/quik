#pragma once

#include <vector>
#include <lex.hh>

#include "ast.hh"

AstNode *build_node(std::vector<Token> tokens);
int build_tree(std::vector<AstNode *> nodes, AstNode *top, int index = 0);
