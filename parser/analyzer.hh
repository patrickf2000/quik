#pragma once

#include "ast.hh"

void find_variables(AstNode *top);
void find_assign(AstNode *top);
void check_return(AstNode *top);
