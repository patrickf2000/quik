#pragma once

#include <ast.hh>

#include "aasm.hh"

void aasm_build_extern(AstNode *node, AsmNode *scope);
void aasm_build_func(AstNode *node, AsmNode *scope);
void aasm_build_func_call(AstNode *node, AsmNode *scope);
void aasm_build_ret(AstNode *node, AsmNode *scope);
