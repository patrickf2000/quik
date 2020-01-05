#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "optimize.hh"

//The constructor
Optimize::Optimize(AstScope *top) {
	tree = top;
}

//Scans the tree for uncalled functions
void Optimize::rm_uncalled_funcs() {
	//Needed vectors
	std::vector<std::string> f_calls;
	f_calls.push_back("main");
	
	//Now, build a list of function calls
	for (auto node : tree->children) {
		if (node->type == AstType::FuncDec) {
			scan_tree(node, AstType::FuncCall, &f_calls);
		}
	}
	
	//Finally, remove anything not called
	std::vector<int> to_remove;
	
	for (int i = 0; i<tree->children.size(); i++) {
		auto node = static_cast<AstAttrNode *>(tree->children.at(i));
		
		if (node->type == AstType::ExternFunc || node->type == AstType::FuncDec) {
			auto name = node->get_name();
			
			if (std::find(f_calls.begin(), f_calls.end(), name) == f_calls.end()) {
				to_remove.push_back(i);
			}
		}
	}
	
	int rm = 0;
	for (auto i : to_remove) {
		tree->children.erase(tree->children.begin() + (i-rm));
		++rm;
	}
}

//Scans a tree for a particular node
//TODO: Maybe a way we can clean this up?
void Optimize::scan_tree(AstNode *node, AstType t, std::vector<std::string> *vals) {
	if (node->type == t) {
		AstAttrNode *n = static_cast<AstAttrNode *>(node);
		vals->push_back(n->get_name());
	}
	
	for (auto n : node->children) {
		if (n->type == t) {
			AstAttrNode *n2 = static_cast<AstAttrNode *>(n);
			vals->push_back(n2->get_name());
		}
		
		if (n->children.size() > 0)
			scan_tree(n, t, vals);
	}
}
