#pragma once

#include <ast.hh>

class OptimizeEngine {
public:
	explicit OptimizeEngine(AstNode *top) {
		tree = top;	
	}

	void remove_extern();
private:
	AstNode *tree;
};
