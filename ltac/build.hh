#pragma once

#include <ast.hh>

#include "ltac.hh"

AsmFile *build_asm_file(AstNode *top);
void write_asm_file(AsmFile *file);
