#pragma once

#include <ast.hh>

#include "aasm.hh"

AsmNode *aasm_build_extern(AstNode *node);
void aasm_build_func(AstNode *node, AsmNode *scope);
AsmNode *aasm_build_func_call(AstNode *node);
AsmNode *aasm_build_ret(AstNode *node);
