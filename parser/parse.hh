#pragma once

#include <vector>
#include <lex.hh>

#include "ast.hh"

AstNode *build_node(std::vector<Token> tokens);
