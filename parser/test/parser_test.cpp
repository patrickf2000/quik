#include <iostream>
#include <string>
#include <fstream>

#include <lex.hh>
#include <ast.hh>

#include "parse.hh"

int main(int argc, char *argv[]) {
	std::vector<std::string> contents;
	contents.push_back("include stdio");
	contents.push_back("include stdlib");
	contents.push_back("fn sayHello");
	contents.push_back("puts(\"Hello!\")");
	contents.push_back("end");
	contents.push_back("fn main");
	contents.push_back("puts(\"Hello, world!\")");
	contents.push_back("puts(\"UPL!!\")");
	contents.push_back("end");
	
	std::vector<AstNode *> all_nodes;
	AstNode *top = new AstNode(AstType::Global);
	
	for (auto ln : contents) {
		auto tokens = tokenize(ln);
		AstNode *node = build_node(tokens);
		
		if (node != nullptr) {
			all_nodes.push_back(node);
		}
	}
	
	build_tree(all_nodes, top);
	print_tree(top);
	
	//AstNode *file = new AstNode(AstType::Global);
	return 0;
}
