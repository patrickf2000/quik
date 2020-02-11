#pragma once

#include <ast.hh>

#include "ltac.hh"

void aasm_build_extern(AstNode *node, AsmFile *scope);
void aasm_build_func(AstNode *node, AsmFile *scope);
void aasm_build_func_call(AstNode *node, AsmFile *scope);
void aasm_build_ret(AstNode *node, AsmFile *scope);
