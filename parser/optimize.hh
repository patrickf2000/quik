#pragma once

#include <vector>
#include <string>

#include <ast.hh>

class Optimize {
public:
	explicit Optimize(AstScope *top);
	void rm_uncalled_funcs();
protected:
	void scan_tree(AstNode *node, AstType t, std::vector<std::string> *vals);
private:
	AstScope *tree;
};
