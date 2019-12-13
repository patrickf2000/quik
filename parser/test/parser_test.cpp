#include <iostream>
#include <string>
#include <fstream>

#include <lex.hh>
#include <ast.hh>

#include "build.hh"

int main(int argc, char *argv[]) {
	auto lines = load_source(argv[1]);
	AstNode *top = build_ast(lines);
	print_tree(top);
	return 0;
}
