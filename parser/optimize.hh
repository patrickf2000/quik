#pragma once

#include <ast.hh>

class Optimize {
public:
	explicit Optimize(AstScope *top);
private:
	AstScope *tree;
};
