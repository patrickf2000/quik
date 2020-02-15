#pragma once

#include <vector>
#include <string>

#include <ast.hh>

class OptimizeEngine {
public:
	explicit OptimizeEngine(AstNode *top) {
		tree = top;	
	}

	void remove_uncalled();
protected:
	void scan_tree(AstNode *node, AstType t, std::vector<std::string> *vals);
private:
	AstNode *tree;
};
