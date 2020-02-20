#pragma once

#include "ltac.hh"
#include "ast.hh"

class LTAC_Builder {
public:
	LTAC_Builder() {}
	LtacFile *build_file(AstNode *top);
protected:
	void assemble(AstNode *top, LtacFile *file);
};
