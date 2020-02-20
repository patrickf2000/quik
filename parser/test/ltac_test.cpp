#include <iostream>
#include <string>

#include <build.hh>
#include <ltac/ltac.hh>
#include <ast.hh>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "Fatal: No input file." << std::endl;
		return 1;
	}
	
	auto lines = load_source(argv[1]);
	AstNode *top = build_ast(lines, false, false);
	
	delete top;
	
	return 0;
}
