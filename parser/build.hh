#pragma once

#include <string>
#include <vector>

#include "types.hh"
#include "lex.hh"
#include "ast.hh"

std::vector<Line> load_source(const char *path);
AstNode *build_ast(std::vector<Line> lines, bool fail = true);
