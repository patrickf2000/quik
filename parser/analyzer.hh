#pragma once

#include "ast.hh"

void find_id(AstNode *top);
void find_variables(AstNode *top);
void find_assign(AstNode *top);
void find_cond(AstNode *top);
void check_return(AstNode *top);
