#include <iostream>
#include <string>

#include <ast.hh>
#include <build.hh>

#include "../ltac_build.hh"
#include "../ltac.hh"

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "No input specified!" << std::endl;
		return 1;
	}
	
	auto lines = load_source(argv[1]);
	AstNode *top = build_ast(lines, true, false);
	
	AsmFile *file = build_asm_file(top);
	write_asm_file(file);
	
	return 0;
}
