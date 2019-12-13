#include <iostream>
#include <cstdlib>

#include <ast.hh>
#include <build.hh>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "Fatal: No input specified!" << std::endl;
		std::exit(1);	
	}
	
	auto lines = load_source(argv[1]);
	AstNode *node = build_ast(lines);
	print_tree(node);
	
	return 0;
}
