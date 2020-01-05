#include <iostream>
#include <string>
#include <vector>
#include <map>
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
	for (int i = 0; i<tree->children.size(); i++) {
		auto node = static_cast<AstAttrNode *>(tree->children.at(i));
		
		if (node->type == AstType::ExternFunc || node->type == AstType::FuncDec) {
			auto name = node->get_name();
			
			if (std::find(f_calls.begin(), f_calls.end(), name) == f_calls.end()) {
				tree->children.erase(tree->children.begin() + i);
			}
		}
	}
}

//This checks for small functions and inlines them if appropriate it
//Algorithm:
// 1) Scan the tree for all functions (excluding main)
// 2) If the size (the number of children in the scope) is less than or equal
//		to 5, then select it for inling
// 3) Iterate through the tree
// 4) If we have a function call that matches an inlined function, replace it
//		with the nodes of the function and remove the call
void Optimize::inline_funcs() {
	const int i_size = 5;	//The number of lines to qualify for inlining
	std::map<std::string, AstNode *> to_inline;
	
	for (auto node : tree->children) {
		if (node->type == AstType::FuncDec) {
			auto scope = node->children.at(0);
			
			if (count_children(scope) <= 5) {
				auto fn = static_cast<AstAttrNode *>(node);
				if (fn->get_name() == "main")
					continue;
					
				to_inline[fn->get_name()] = scope;
			}
		}
	}
	
	//TODO: I'm not sure if this works in loops or conditionals
	for (auto node : tree->children) {
		if (node->type != AstType::FuncDec)
			continue;
		
		//Get all the nodes below the scope
		auto scope = node->children.at(0);
		
		for (int i = 0; i<scope->children.size(); i++) {
			auto n = scope->children.at(i);
			if (n->type != AstType::FuncCall)
				continue;
				
			auto fc = static_cast<AstAttrNode *>(n);
			if (to_inline.find(fc->get_name()) == to_inline.end())
				continue;
			
			scope->children.erase(scope->children.begin() + i);
			
			auto to_insert = to_inline[fc->get_name()];
			for (int j = 0; j<to_insert->children.size(); j++) {
				auto current = to_insert->children.at(j);
				if (current->type == AstType::Return)
					continue;
				
				scope->children.insert(scope->children.begin() + i + j, current);
			}
		}
	}
	
	rm_uncalled_funcs();
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

//Counts all children in a given tree
int Optimize::count_children(AstNode *node) {
	int count = node->children.size();
	
	if (count > 0) {
		for (auto c : node->children) {
			switch (c->type) {
				case AstType::While:
				case AstType::If:
				case AstType::Elif:
				case AstType::Else: count += count_children(c); break;
			}
		}
	}
	
	return count;
}
